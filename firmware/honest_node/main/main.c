/* honest_node - W-MSR consensus + CSI fingerprint trust layer.
 *
 * Assembled across the build guide:
 *   Steps 4-8   ESP-NOW link, packet format, neighbour table
 *   Steps 9-12  consensus, sensor anchor, W-MSR trim, telemetry
 *   Steps 18-27 CSI capture, fingerprints, clustering, trust fusion
 *
 * Start at Step 4 with the smallest thing that works and grow it.
 */
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

static void example_wifi_init(void)
    {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE));
    #if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK( esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
    #endif
}
static void print_station_mac(void){
    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA,mac));
    printf("Station MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        (unsigned)mac[0], (unsigned)mac[1], (unsigned)mac[2],
        (unsigned)mac[3], (unsigned)mac[4], (unsigned)mac[5]);
}
static void print_wifi_channel(void){
    uint8_t primary;
    wifi_second_chan_t second;
    ESP_ERROR_CHECK(esp_wifi_get_channel(&primary, &second));
    printf("Wi-Fi primary channel: %u\n", (unsigned)primary);
}
void app_main(void)
{
    printf("honest_node: skeleton. Begin at Step 4.\n");
    status_init();
    status_rgb(false, true, false);
    ESP_ERROR_CHECK(nvs_flash_init());
    example_wifi_init();
    print_wifi_channel();
    print_station_mac();

    for (;;) vTaskDelay(pdMS_TO_TICKS(1000));
    
}
