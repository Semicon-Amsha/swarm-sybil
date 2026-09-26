/* attacker_node - Sybil / forge / replay injector.
 *
 * A test harness. It attacks ONE thing: the 29-byte consensus protocol
 * on your own boards. No infrastructure Wi-Fi, association, or crypto.
 *
 *   Step 14  forge one false value under the real identity
 *   Step 15  Sybil: many spoofed MACs from one radio
 *   Step 17  replay captured packets after a delay
 *
 * Console: ATK:SYBIL:<n>:<val> | ATK:FORGE:<val> | ATK:REPLAY:<ms> | ATK:STOP
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("attacker_node: skeleton. Build from Step 14.\n");
    for (;;) vTaskDelay(pdMS_TO_TICKS(1000));
}
