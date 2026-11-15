# Task 2 — CAN Signal Definition Document

All messages use standard 11-bit identifiers, DLC = 8, Intel/little-endian
byte order, unsigned 16-bit signal at start bit 0. Bytes 2–7 are reserved.

| CAN ID | Message | Signal | Start | Length | Type | Factor | Offset | Min | Max | Unit |
|---:|---|---|---:|---:|---|---:|---:|---:|---:|---|
| 0x100 | Vehicle_Speed | VehicleSpeed | 0 | 16 | unsigned | 0.1 | 0 | 0 | 120 | km/h |
| 0x101 | Engine_RPM | EngineRPM | 0 | 16 | unsigned | 1 | 0 | 800 | 5000 | rpm |
| 0x102 | Coolant_Temp | CoolantTemperature | 0 | 16 | unsigned | 0.1 | -40 | 20 | 120 | degC |
| 0x103 | Fuel_Level | FuelLevel | 0 | 16 | unsigned | 0.5 | 0 | 0 | 100 | % |
| 0x104 | Battery_Status | BatteryVoltage | 0 | 16 | unsigned | 0.01 | 0 | 11 | 15 | V |
| 0x105 | Ambient_Temp | AmbientTemperature | 0 | 16 | unsigned | 0.1 | -40 | -20 | 60 | degC |

Physical = raw × factor + offset.
Raw = (physical − offset) / factor.

Examples:
- 65 km/h -> raw 650 -> bytes `8A 02`.
- 2450 rpm -> raw 2450 -> bytes `92 09`.
- 87 °C coolant -> raw 1270 -> bytes `F6 04`.
- 72% fuel -> raw 144 -> bytes `90 00`.
- 12.50 V -> raw 1250 -> bytes `E2 04`.
- 28 °C ambient -> raw 680 -> bytes `A8 02`.

The IDs, scaling and layout are an original teaching design for this exercise,
not an OEM-standard allocation.
