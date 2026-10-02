idf_component_register(
    SRCS "main.c"
         "../../common/wmsr.c"
         "../../common/csi_fp.c"
         "../../common/status_led.c"
    INCLUDE_DIRS "." "../../common"
    REQUIRES esp_wifi esp_timer nvs_flash esp_event driver esp_ringbuf esp_adc
)
