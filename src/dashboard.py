#!/usr/bin/env python3
"""Live DBC-decoded terminal dashboard using a Linux SocketCAN raw socket."""
import socket
import struct
import sys
import time
from pathlib import Path
import cantools

DBC = Path(__file__).resolve().parents[1] / "vehicle_network.dbc"
MESSAGE_TO_SIGNAL = {
    0x100: ("VehicleSpeed", "km/h"),
    0x101: ("EngineRPM", "rpm"),
    0x102: ("CoolantTemperature", "°C"),
    0x103: ("FuelLevel", "%"),
    0x104: ("BatteryVoltage", "V"),
    0x105: ("AmbientTemperature", "°C"),
}

def main():
    interface = sys.argv[1] if len(sys.argv) > 1 else "vcan0"
    db = cantools.database.load_file(str(DBC))
    sock = socket.socket(socket.PF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
    sock.bind((interface,))
    values = {}
    last_update = {}

    print(f"Listening on {interface}; press Ctrl+C to stop.")
    try:
        while True:
            packet = sock.recv(16)
            can_id, dlc, data = struct.unpack("=IB3x8s", packet)
            can_id &= socket.CAN_EFF_MASK if hasattr(socket, "CAN_EFF_MASK") else 0x1FFFFFFF
            can_id &= 0x7FF
            if can_id not in MESSAGE_TO_SIGNAL:
                continue
            message = db.get_message_by_frame_id(can_id)
            decoded = message.decode(data[:dlc])
            signal, unit = MESSAGE_TO_SIGNAL[can_id]
            values[signal] = decoded[signal]
            last_update[signal] = time.strftime("%H:%M:%S")

            print("\033[2J\033[H", end="")
            print("=" * 58)
            print("        KAPEXLAB VEHICLE INFORMATION DASHBOARD")
            print(f"        Interface: {interface} | Updated: {time.strftime('%H:%M:%S')}")
            print("=" * 58)
            rows = [
                ("Vehicle Speed", "VehicleSpeed", "km/h", "{:.1f}"),
                ("Engine RPM", "EngineRPM", "rpm", "{:.0f}"),
                ("Coolant Temperature", "CoolantTemperature", "°C", "{:.1f}"),
                ("Fuel Level", "FuelLevel", "%", "{:.1f}"),
                ("Battery Voltage", "BatteryVoltage", "V", "{:.2f}"),
                ("Ambient Temperature", "AmbientTemperature", "°C", "{:.1f}"),
            ]
            for label, key, unit, fmt in rows:
                shown = fmt.format(values[key]) if key in values else "--"
                print(f"{label:<25}: {shown:>8} {unit:<5}  {last_update.get(key, 'waiting')}")
            print("-" * 58)
            print("DBC decoding active | Ctrl+C to exit")
    except KeyboardInterrupt:
        print("\nDashboard stopped.")
    finally:
        sock.close()

if __name__ == "__main__":
    main()
