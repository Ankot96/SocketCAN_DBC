# Task 1 — Understanding DBC Files (one-page summary)

A DBC (Database CAN) file is a human-readable database description used to
interpret the payload bytes of CAN messages. A CAN frame contains an identifier,
a data length and raw data bytes. By itself, the payload does not explain which
bits represent vehicle speed, engine RPM or temperature. The DBC provides that
meaning.

A DBC is required because CAN communication transports compact binary data.
Different ECUs and diagnostic tools need a common definition of message IDs,
signal positions, lengths, byte order, signedness, scaling, offsets, limits and
units. Without a matching database, an engineer may see hexadecimal bytes but
cannot reliably convert them into engineering values.

A DBC commonly contains:
- Network nodes (ECUs) that transmit or receive messages.
- Message definitions: CAN identifier, message name and payload length.
- Signal definitions: name, start bit, bit length and byte order.
- Value interpretation: signed/unsigned type, factor, offset, minimum,
  maximum and unit.
- Optional comments, enumerated value tables and transmitter/receiver details.

A CAN analysis tool loads a DBC and associates each received CAN identifier
with its message definition. It extracts each signal's bits, applies byte order
and signedness, then calculates the physical value using:
Physical value = raw value × factor + offset.
The tool can then show decoded names, values, units, graphs and message
activity. The DBC must match the actual transmitter layout; an incorrect start
bit, length, factor, offset or unit can produce misleading displayed values.

In this assignment, `vehicle_network.dbc` defines six messages for vehicle
speed, engine RPM, coolant temperature, fuel level, battery voltage and ambient
temperature. The DBC is used by `cantools` in the validation utility and live
dashboard, and can also be imported into a DBC-capable CAN analysis application.

A DBC is a signal-interpretation database, not a transport protocol and not a
description of the physical CAN wiring. SocketCAN carries the frames; the DBC
explains what the bits mean.
