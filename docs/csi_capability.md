# CSI capability verification (Step 2)

IDF version: v5.2.7
Chip:        ESP32-D0WD-V3 (genuine WROOM-32)
Tool:        StevenMHernandez/ESP32-CSI-Tool, `passive` variant
Fixes needed on this IDF: add `#include "esp_mac.h"`; enable
CONFIG_ESP_WIFI_CSI_ENABLED=y in the project sdkconfig.

## Result

CSI_DATA rows stream from ambient traffic. Guard-band zeros visible in the
middle of every bracket, and different transmitters show clearly different
signatures - both exactly what the project relies on.

Sample captured line (board 1):

    CSI_DATA,PASSIVE,34:CF:F6:94:47:D3,-34,11,...,[30 -32 1 0 0 10 1 9 ... ]

## Board pass/fail log

| Board label | Serial / MAC        | CSI? | Notes                    |
|-------------|---------------------|------|--------------------------|
| board-1     | e8:6b:ea:dc:64:04   | PASS | verified via passive     |
| board-2     |                     |      | pending (not yet owned)  |
