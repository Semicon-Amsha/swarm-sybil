# Physical-Layer Sybil Defence on ESP32

Detecting spoofed identities in a wireless swarm by listening to the radio
channel (CSI), not the message. Eight ESP32 nodes reach agreement with
W-MSR consensus; one attacker forges many identities from a single radio;
the honest nodes fingerprint the channel, spot the shared radio, and
exclude the fakes.

See docs/ for the build guide. Work through it one step at a time:
build one thing -> test it -> commit it -> build the next thing.

## Layout

    firmware/common/          shared code: protocol, consensus, fingerprints
    firmware/honest_node/     the swarm node firmware
    firmware/attacker_node/   Sybil / forge / replay harness
    dashboard/                Python + browser live view
    analysis/                 metrics, plots, calibration, simulation
    data/experiments/         one CSV per run (git-ignored bulk)
    docs/                     conditions logs, CSI dumps, findings

## Toolchain

- ESP-IDF v5.2.x  (pinned; do not use v6.x)
- Python 3.10+    (dashboard and analysis)

## Status

- [x] Step 2: CSI capability verified on hardware (see docs/)
- [ ] Step 1..: firmware build in progress
