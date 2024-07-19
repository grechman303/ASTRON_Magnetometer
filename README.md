# ASTRON_Magnetometer
Software for the Magnetometer developed for ASTRON as part of the HTSM summer school 2024.

This repository includes the following files
- **Supplementary Documents** - this folder contains relevant documentation (in pdf) for the _FGM-3 fluxgate magnetometer sensor_, the _Dragino LPS8v2 gateway_ and the layout of the _Arduino MKR WAN 1310_ microcontroller.
- **sensor_read_and_send**
    - **sensor_read_and_send.ino** - the code for the Arduino to periodically read data from the sensors and transmit it via LoRaWAN.
    - **arduino_secrets.h** - the necessary keys to connect to the network server.
- **basic-decode.js** - JavaScript code to decode the uplink message received on ChirpStack. It must be added in the "Codec" section of the created Device Profile in ChirpStack.
- **magnetometer-data-processing-flow.json** - JSON representation of the Node-RED flow to process/format data from ChirpStack and send it to the InfluxDB database.
- **Software Setup Documentation.pdf** - documentation detailing how to setup the software of the system on windows.
