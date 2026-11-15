# KAPEXLab Problem Statement 3 — SocketCAN + DBC

This package implements a simulated Vehicle Information Network using Linux
SocketCAN (`vcan0`), a native C transmitter, a DBC database, a Python decoder /
terminal dashboard, and validation utilities.

## Architecture
C vehicle-data generator -> SocketCAN transmitter -> vcan0 -> raw capture /
DBC decoder -> live dashboard.

## 1. Install prerequisites (Ubuntu)
```bash
sudo apt update
sudo apt install -y build-essential can-utils python3 python3-pip
python3 -m pip install --user cantools
```

## 2. Create and start the virtual CAN interface
```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set vcan0 up
ip -details link show vcan0
```
If `vcan0` already exists, do not add it again; just run:
```bash
sudo ip link set vcan0 up
```

## 3. Build and run the C transmitter
From this directory:
```bash
gcc -Wall -Wextra -O2 src/can_transmitter.c -o can_transmitter
./can_transmitter vcan0
```
Stop with Ctrl+C. It transmits six messages every 100 ms.

## 4. Observe raw traffic (second terminal)
```bash
candump -L vcan0
```
or:
```bash
cansniffer -c vcan0
```

## 5. Run DBC validation
```bash
python3 src/validate_dbc.py
```
Expected: all six messages decode to the original engineering values.

## 6. Run the live dashboard (third terminal)
```bash
python3 src/dashboard.py vcan0
```
The dashboard uses the DBC to decode frames and refreshes as messages arrive.
Press Ctrl+C to exit.

## 7. Open the DBC in a CAN tool
Open `vehicle_network.dbc` in SavvyCAN (or another DBC-capable tool).
For a vcan0 setup, configure the tool to receive from the Linux SocketCAN
interface if supported by your installation. Alternatively, save a candump
log and import it into the tool. Capture screenshots of:
- raw frames (candump)
- decoded signals / graph view (CAN tool)
- live terminal dashboard

## Signal layout
All messages are standard 11-bit CAN IDs, DLC 8, Intel/little-endian.
The signal is placed at start bit 0, length 16. Unused bytes are reserved.
Scaling and offsets are specified in the DBC.

| ID | Message | Signal | Factor | Offset | Unit |
|---|---|---|---:|---:|---|
| 0x100 | Vehicle_Speed | VehicleSpeed | 0.1 | 0 | km/h |
| 0x101 | Engine_RPM | EngineRPM | 1 | 0 | rpm |
| 0x102 | Coolant_Temp | CoolantTemperature | 0.1 | -40 | °C |
| 0x103 | Fuel_Level | FuelLevel | 0.5 | 0 | % |
| 0x104 | Battery_Status | BatteryVoltage | 0.01 | 0 | V |
| 0x105 | Ambient_Temp | AmbientTemperature | 0.1 | -40 | °C |

Raw value = (physical value - offset) / factor.
Physical value = raw value * factor + offset.

## Notes
- This is a teaching simulation, not an OEM vehicle database.
- CAN IDs and signal scaling are chosen for this assignment and are not
  claimed to follow any manufacturer's specification.
- `vcan0` has no physical-layer electrical signaling and cannot connect to a
  real CAN bus without suitable hardware.
