
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "status_led.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "status_led.h"
#include "esp_now.h"
#include "string.h"
#include "protocol.h"
#include "freertos/semphr.h"
#define MAX_DEVICES 8
#define ROLE_SENDER 1
#define ESP_NOW_CHANNEL 6

const uint8_t broadcast_addr[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t g_my_mac[6];

static volatile uint32_t g_received_count = 0;
static volatile uint32_t g_baseline_seq = 0;
static volatile uint32_t g_last_seq = 0;
static volatile bool g_have_baseline = false;
static volatile uint32_t g_csi_count = 0;
static void csi_probe_cb(void *ctx, wifi_csi_info_t *info);

typedef struct
{
    uint8_t id[6];
    float state;
    float alpha; // trust weight - stays 1.0 for now
    int64_t last_seen_us;
    uint32_t pkt_count;
    bool valid;
} neighbor_t;

    static neighbor_t g_nb[MAX_DEVICES];
static SemaphoreHandle_t g_mtx;

static int slot_of_locked(const uint8_t id[6])
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (!g_nb[i].valid)
        {
            continue;
        }
        if (memcmp(g_nb[i].id, id, 6) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int alloc_slot_locked(const uint8_t id[6])
{
    int index = slot_of_locked(id);
    if (index != -1)
    {
        return index;
    }
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (!g_nb[i].valid){
            memcpy(g_nb[i].id, id, 6);

            g_nb[i].valid = true;
            g_nb[i].state = 0.0f;
            g_nb[i].alpha = 1.0f;
            g_nb[i].last_seen_us = 0;
            g_nb[i].pkt_count = 0;

            return i;
        }
    }
    return -1;
}

static void wmsr_on_packet(const swarm_pkt_t *pkt){
    xSemaphoreTake(g_mtx, portMAX_DELAY);
    int idx = alloc_slot_locked(pkt->src_id);

    if(idx >= 0){
        g_nb[idx].state = pkt->state;
        g_nb[idx].last_seen_us = esp_timer_get_time();
        g_nb[idx].pkt_count++;
    }

    xSemaphoreGive(g_mtx);
}


#if ROLE_SENDER

static void on_sent(const uint8_t *mac_addr,
                    esp_now_send_status_t status)
{
    printf("ESP-NOW send: %s\n",
           status == ESP_NOW_SEND_SUCCESS
               ? "success"
               : "failed");
}

static void sender_task(void *arg)
{
    swarm_pkt_t pkt = {0};
    pkt.magic = PKT_MAGIC;
    memcpy(pkt.src_id, g_my_mac, 6);

    for (;;)
    {
        esp_err_t err = esp_now_send(
            broadcast_addr,
            (const uint8_t *)&pkt,
            sizeof(pkt));

        if (err != ESP_OK)
        {
            printf("ESP-NOW send request failed: %s\n", esp_err_to_name(err));
        }

        pkt.seq++;
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
#endif

#if !ROLE_SENDER
static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    swarm_pkt_t pkt;
    if (len != sizeof(pkt))
    {
        return;
    }

    memcpy(&pkt, data, sizeof(pkt));

    if (pkt.magic != PKT_MAGIC)
    {
        return;
    }

    wmsr_on_packet(&pkt);

    if (!g_have_baseline)
    {
        g_baseline_seq = pkt.seq;
        g_have_baseline = true;
    }
    g_last_seq = pkt.seq;
    g_received_count++;

    printf("from %02X:%02X  seq=%lu\n",
           info->src_addr[4], info->src_addr[5],
           (unsigned long)pkt.seq);
}
#endif

static void example_wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
#if !ROLE_SENDER
    wifi_csi_config_t csi_cfg = {
        .lltf_en = true,
        .htltf_en = false,
        .stbc_htltf2_en = false,
        .ltf_merge_en = true,
        .channel_filter_en = false, /* raw, no hardware smoothing */
        .manu_scale = false,
    };
    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&csi_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(csi_probe_cb, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_csi(true));
#endif
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
#if ROLE_SENDER
    ESP_ERROR_CHECK(esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_6M));
#endif
#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
#endif
}
static void print_station_mac(void)
{

    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, g_my_mac));
    printf("Station MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
           (unsigned)g_my_mac[0], (unsigned)g_my_mac[1], (unsigned)g_my_mac[2],
           (unsigned)g_my_mac[3], (unsigned)g_my_mac[4], (unsigned)g_my_mac[5]);
}
static void print_wifi_channel(void)
{
    uint8_t primary;
    wifi_second_chan_t second;
    ESP_ERROR_CHECK(esp_wifi_get_channel(&primary, &second));
    printf("Wi-Fi primary channel: %u\n", (unsigned)primary);
}
static void init_esp_now_broadcast(void)
{
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_peer_info_t peer = {0};
#if ROLE_SENDER
    ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));
#else
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_recv));
#endif
    memcpy(peer.peer_addr, broadcast_addr, sizeof(broadcast_addr));
    peer.channel = ESP_NOW_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
    printf("ESP-NOW ready: broadcast peer registered\n");
}
static void csi_probe_cb(void *ctx, wifi_csi_info_t *info)
{
    g_csi_count++;
}
static void link_stat_task(void *arg)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
        if (!g_have_baseline)
        {
            continue;
        }
        uint32_t expected = g_last_seq - g_baseline_seq + 1;
        uint32_t actual = g_received_count;

        float loss_percent = (1 - ((float)actual / expected)) * 100;
        float csi2pktRto = g_csi_count / (float)g_received_count;
        printf("link: rx=%lu  expected=%lu  loss=%.2f%%  CSI2PKT=%.2f\n",
               (unsigned long)actual, (unsigned long)expected, loss_percent, csi2pktRto);
    }
}

static void print_neighbours_task(void *arg)
{
    for (;;)
    {
        if (xSemaphoreTake(g_mtx, portMAX_DELAY) == pdTRUE)
        {
            printf("neighbours=");

            for (int i = 0; i < MAX_DEVICES; i++)
            {
                if (g_nb[i].valid)
                {
                    printf(" [%02X:%02X seq=%lu]",
                           g_nb[i].id[4],
                           g_nb[i].id[5],
                           (unsigned long)g_nb[i].pkt_count);
                }
            }

            printf("\n");

            xSemaphoreGive(g_mtx);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    printf("honest_node: skeleton. Begin at Step 4.\n");
    printf("sizeof(swarm_pkt_t) = %d\n", (int)sizeof(swarm_pkt_t));
    status_init();
    status_rgb(false, true, false);
    ESP_ERROR_CHECK(nvs_flash_init());
    g_mtx = xSemaphoreCreateMutex();

    if(g_mtx == NULL){
        printf("Failed to create Mutex\n");
        return;
    }

    example_wifi_init();
    print_wifi_channel();
    init_esp_now_broadcast();
    print_station_mac();


#if ROLE_SENDER
    xTaskCreate(sender_task, "sender", 4096, NULL, 5, NULL);
#endif

#if !ROLE_SENDER
    xTaskCreate(link_stat_task, "receiver", 4096, NULL, 5, NULL);

    xTaskCreate(print_neighbours_task,
                "print_neighbours",
                4096,
                NULL,
                5,
                NULL);
#endif

    for (;;)
        vTaskDelay(pdMS_TO_TICKS(1000));
}
