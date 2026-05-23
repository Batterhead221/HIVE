# HIVE_V1
ESP32-based outlet/circuit presence monitor using heartbeat-based ON/OFF packets

This project detects whether a remote powered outlet/circuit is ON or OFF without directly measuring mains voltage. A small ESP32 node is powered from the outlet being monitored. If the outlet has power, the node sends periodic wireless heartbeats. If the outlet loses power, the heartbeats stop and the gateway marks that circuit as OFF/missing.

## Project Status

Prototype working.

Current proof-of-concept:

- ESP32-S3 outlet node sends heartbeat packets over ESP-NOW
- ESP32-C6 gateway receives packets
- Gateway reports node state over USB serial
- If heartbeat stops for the timeout period, gateway reports the node as OFF/missing

## System Concept

Wall outlet / monitored circuit
        ↓
USB wall adapter
        ↓
ESP32-S3 outlet node
        ↓ ESP-NOW heartbeat
ESP32-C6 gateway
        ↓ USB serial
Computer / future dashboard

Current Hardware:

Gateway

Custom ESP32-C6 gateway PCB.

Main parts:

* ESP32-C6-WROOM-1-N8 module
* USB-C power/data
* AP2112K-3.3 voltage regulator
* USBLC6-2P6 USB ESD protection
* BOOT and RESET buttons
* RGB status LED
* Power LED
* UART debug pads
* 4-layer PCB with internal GND plane

Outlet Node

Prototype node:

* Seeed XIAO ESP32-S3
* External 2.4 GHz antenna
* USB wall adapter

The XIAO ESP32-S3 must have the antenna installed. 
Without the antenna, ESP-NOW transmission may appear to succeed in software while the gateway receives nothing.

Example Node IDs:
KITCHEN
GARAGE
BASEMENT
CIRCUIT_01

Example output:
KITCHEN = ON | last seen 961 ms ago
KITCHEN = OFF / missing | last seen 45960 ms ago

Current defaults:
Heartbeat interval: 5 seconds
Gateway timeout: 20 seconds
ESP-NOW channel: 1

This prototype does not connect directly to mains voltage. The outlet node is powered by an off-the-shelf USB wall adapter.