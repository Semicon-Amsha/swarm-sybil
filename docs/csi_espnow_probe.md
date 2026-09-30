# CSI-on-ESP-NOW probe — Step 7

Date: 2026-09-27
IDF version: 5.2.7
Channel: 6

Initial result with default ESP-NOW rate: CSI2PKT = 0.00 (callback never fired)
Fix: pinned sender's ESP-NOW rate to WIFI_PHY_RATE_6M via
     esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_6M).
     This matches the receiver's lltf_en=true / htltf_en=false CSI config,
     which expects legacy-rate frames, not the default HT rate.

After fix: CSI2PKT = 1.00, steady across 4+ windows (100+ packets).
Packet loss also rose from ~0% (Step 6) to ~4%, consistent with the added
per-packet work the CSI callback introduces.
