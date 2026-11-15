# AI Usage Report — KAPEXLab Problem Statement 3

## AI tool selected
ChatGPT (OpenAI), used as an engineering assistant for initial DBC design,
code drafting, review and documentation.

## Prompts used
1. "Explain the purpose and structure of a DBC file for a beginner working
   with automotive CAN."
2. "Design standard 11-bit CAN messages for speed, RPM, coolant temperature,
   fuel level and battery voltage. Specify start bit, length, byte order,
   scaling, offset, range and unit."
3. "Generate a DBC file for the selected signals using Intel byte order and
   unsigned 16-bit signals."
4. "Write a native Linux SocketCAN C transmitter that periodically sends
   changing vehicle values on vcan0."
5. "Create a Python cantools validation script and a live SocketCAN dashboard."
6. "Review the DBC and identify inconsistencies in scaling, ranges, IDs and
   raw-value encoding."

## AI-generated outputs
The AI-assisted work produced an initial message layout, DBC definitions,
SocketCAN transmitter structure, validation procedure, decoder/dashboard
approach, report outline and challenge ideas.

## Corrections and verification performed
- Checked each DBC message identifier against the transmitter's hexadecimal ID.
- Checked all messages use DLC 8 and the intended 16-bit little-endian layout.
- Checked the physical conversion formula: physical = raw × factor + offset.
- Verified representative values by encoding and decoding through cantools.
- Ensured coolant and ambient offsets are included in raw-value calculation.
- Kept the DBC physical ranges consistent with the assignment and extension.
- Treated the virtual CAN interface as a software simulation, not a physical bus.

## Lessons learned
- Raw CAN payload bytes do not have engineering meaning without a shared
  message/signal definition.
- Scaling and offset are essential to decode raw integer values correctly.
- A DBC can be tested programmatically before it is imported into a GUI tool.
- AI-generated code and databases must be compiled, executed and validated;
  generated output should not be accepted without verification.
- A DBC describes signal interpretation, while SocketCAN handles frame
  transport on Linux.
