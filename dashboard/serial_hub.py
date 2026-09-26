#!/usr/bin/env python3
"""serial_hub.py - read every node's serial port, log CSV, serve dashboard.

Built at Step 29. Reads all COM/ttyUSB ports in threads, logs one CSV,
and broadcasts parsed telemetry to the browser over WebSocket.

    python serial_hub.py --label E3_SYBIL_N5_V09 --attacker COM7
"""
# TODO Step 29: one reader thread per port; Flask-SocketIO; attack cmds
