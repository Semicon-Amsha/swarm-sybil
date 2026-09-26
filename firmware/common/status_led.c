/* status_led.c - RGB status LED helper.  Filled in at Step 3.
 * GPIO 25 = R, 26 = G, 27 = B, common-cathode, 220 ohm each.
 */


#include "status_led.h"
#include "driver/gpio.h"

// #define LED_R  GPIO_NUM_25
// #define LED_G  GPIO_NUM_26
// #define LED_B  GPIO_NUM_27

// void status_init(void)
// {
//     gpio_config_t c = {
//         .pin_bit_mask = (1ULL<<LED_R)|(1ULL<<LED_G)|(1ULL<<LED_B),
//         .mode = GPIO_MODE_OUTPUT,
//     };
//     gpio_config(&c);
// }

// void status_rgb(int r, int g, int b)
// {
//     gpio_set_level(LED_R, r);
//     gpio_set_level(LED_G, g);
//     gpio_set_level(LED_B, b);
// }

#include <stdbool.h>
#define LED_R GPIO_NUM_25
#define LED_G GPIO_NUM_26
#define LED_B GPIO_NUM_27

void status_init(void){
    gpio_config_t c={
        .pin_bit_mask= (1ULL<<LED_R)|(1ULL<<LED_G)|(1ULL<<LED_B),
        .mode= GPIO_MODE_OUTPUT
    };
    gpio_config(&c);
}

void status_rgb(bool r, bool g, bool b){
    gpio_set_level(LED_R, r);
    gpio_set_level(LED_G, g);
    gpio_set_level(LED_B, b);
}