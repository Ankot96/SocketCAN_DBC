#!/usr/bin/env python3
"""Load the DBC, encode test values, decode them, and check round-trip accuracy."""
from pathlib import Path
import cantools

DBC = Path(__file__).resolve().parents[1] / "vehicle_network.dbc"

TESTS = [
    ("Vehicle_Speed", {"VehicleSpeed": 65.0}),
    ("Engine_RPM", {"EngineRPM": 2450}),
    ("Coolant_Temp", {"CoolantTemperature": 87.0}),
    ("Fuel_Level", {"FuelLevel": 72.0}),
    ("Battery_Status", {"BatteryVoltage": 12.5}),
    ("Ambient_Temp", {"AmbientTemperature": 28.0}),
]

def main():
    db = cantools.database.load_file(str(DBC))
    print(f"Loaded DBC: {DBC.name}")
    failures = 0
    for message_name, values in TESTS:
        message = db.get_message_by_name(message_name)
        encoded = message.encode(values)
        decoded = message.decode(encoded)
        signal_name, expected = next(iter(values.items()))
        actual = decoded[signal_name]
        ok = abs(float(actual) - float(expected)) < 1e-6
        print(f"{'PASS' if ok else 'FAIL'} {message_name}: "
              f"{signal_name}={actual} (expected {expected}), "
              f"data={encoded.hex(' ').upper()}")
        if not ok:
            failures += 1
    print(f"\nValidation: {len(TESTS)-failures}/{len(TESTS)} tests passed.")
    if failures:
        raise SystemExit(1)

if __name__ == "__main__":
    main()
