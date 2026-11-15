# Technical Report — AI-Assisted DBC Generation and CAN Data Visualization
**KAPEXLab Automotive Communication Networks — Problem Statement 3**

## 1. Introduction and objective
The objective was to create a simulated vehicle information network, transmit
CAN frames through Linux SocketCAN, describe the frame payloads in a DBC file,
and decode the received signals into engineering values. The network carries
vehicle speed, engine RPM, coolant temperature, fuel level and battery voltage.
Ambient temperature was added as an extension.

## 2. Signal and DBC design
Six standard 11-bit CAN identifiers (0x100–0x105) were assigned. Each message
uses an 8-byte payload and one unsigned 16-bit Intel/little-endian signal at
start bit 0. The DBC specifies factor, offset, physical range and unit. The
conversion is physical = raw × factor + offset. The complete definitions are
provided in `vehicle_network.dbc` and `reports/task2_signal_definition.md`.

## 3. DBC generation and validation
ChatGPT assisted with the initial DBC layout, code skeletons and documentation.
The generated definitions were checked against the transmitter encoding.
`src/validate_dbc.py` loads the DBC, encodes representative physical values,
decodes them and compares the results with the original values. The test
vectors cover all six messages. AI output was treated as a draft and verified
with the cantools library.

## 4. SocketCAN integration
A native C application opens a raw CAN socket, binds it to the selected Linux
CAN interface and writes standard CAN frames. The default interface is
`vcan0`, a virtual CAN interface that allows CAN software to exchange frames
without physical CAN hardware. Vehicle values change over time within the
specified operating ranges. The application sends the six messages every
100 ms.

## 5. Visualization and monitoring
`candump` displays the raw hexadecimal frames. The Python dashboard listens
on the same SocketCAN interface, looks up each received identifier in the DBC,
decodes its signal and displays the engineering value and unit. A DBC-capable
analysis application such as SavvyCAN can also be used to inspect imported
DBC definitions and captured traffic. Screenshots should be inserted below
after running the implementation.

### Screenshot evidence to add
- Figure 1: `ip link show vcan0` showing the virtual interface is up.
- Figure 2: `candump -L vcan0` showing raw CAN traffic.
- Figure 3: dashboard showing decoded signal values.
- Figure 4: CAN analysis tool with DBC-loaded signal names / graph.
- Figure 5: modified DBC factor and the changed decoded speed.

## 6. Challenges and observations
Raw CAN data is a sequence of bytes and cannot be interpreted as engineering
values without a matching database. Changing a DBC factor changes the
decoded physical value even when the transmitted bytes remain unchanged.
Ambient temperature demonstrates the complete process of extending the
network: message definition, DBC entry, transmitter and dashboard update.

## 7. Conclusion
The project demonstrates the workflow from signal definition to CAN message
design, DBC creation, SocketCAN transmission, DBC-based decoding and
visualization. It also demonstrates why DBC validation is important and how
AI can assist engineering work while requiring independent verification.

**Before submission:** add genuine screenshots from your own execution and
record any real issues or corrections observed. Do not present planned
screenshots or unexecuted tests as completed evidence.
