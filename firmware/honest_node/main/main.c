
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
#include "status_led.h"
#include "esp_now.h"
#include "string.h"
#include "protocol.h"

#define ROLE_SENDER 0
#define ESP_NOW_CHANNEL 6
const uint8_t broadcast_addr[ESP_NOW_ETH_ALEN]={ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
static uint8_t g_my_mac[6];

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
    pkt.magic =PKT_MAGIC ;      
    memcpy(pkt.src_id, g_my_mac, 6); 

    for (;;) {
        esp_err_t err = esp_now_send(
            broadcast_addr,
            (const uint8_t *)&pkt,
            sizeof(pkt) 
        );

        if (err != ESP_OK) {
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
    if (len != sizeof(pkt)) {   
        return;
    }
    
    memcpy(&pkt, data, sizeof(pkt));

    if (pkt.magic != PKT_MAGIC) {  
        return;
    }

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
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
    #if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK( esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
    #endif
}
static void print_station_mac(void){
    
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA,g_my_mac));
    printf("Station MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        (unsigned)g_my_mac[0], (unsigned)g_my_mac[1], (unsigned)g_my_mac[2],
        (unsigned)g_my_mac[3], (unsigned)g_my_mac[4], (unsigned)g_my_mac[5]);
}
static void print_wifi_channel(void){
    uint8_t primary;
    wifi_second_chan_t second;
    ESP_ERROR_CHECK(esp_wifi_get_channel(&primary, &second));
    printf("Wi-Fi primary channel: %u\n", (unsigned)primary);
}
static void init_esp_now_broadcast(void){
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_peer_info_t peer = {0};
    #if ROLE_SENDER
       ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));
    #else
       ESP_ERROR_CHECK(esp_now_register_recv_cb(on_recv));
    #endif
    memcpy(peer.peer_addr, broadcast_addr, sizeof(broadcast_addr));
    peer.channel=ESP_NOW_CHANNEL;
    peer.ifidx=WIFI_IF_STA;
    peer.encrypt=false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
    printf("ESP-NOW ready: broadcast peer registered\n");

}


void app_main(void)
{
    printf("honest_node: skeleton. Begin at Step 4.\n");
    printf("sizeof(swarm_pkt_t) = %d\n", (int)sizeof(swarm_pkt_t));
    status_init();
    status_rgb(false, true, false);
    ESP_ERROR_CHECK(nvs_flash_init());
    example_wifi_init();
    print_wifi_channel();
    init_esp_now_broadcast();
    print_station_mac();

    #if ROLE_SENDER
    xTaskCreate(sender_task, "sender", 4096, NULL, 5, NULL);
    #endif

    for (;;) vTaskDelay(pdMS_TO_TICKS(1000));
    
}
