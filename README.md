# AxonOnOnOn

An ESP32 project for research and testing purposes that sends the `Start Recording` command to Axon body and dash cameras.

## What This Is

A tool for research and educational purposes that turns a BLE capable ESP32 device into a `Start Recording` broadcaster that will continuously advertise to supported Axon devices to start recording.

## Building

You'll need:
- ESP32-C3 or compatible ESP32 board
- ESP-IDF v5.x or later
- Basic familiarity with ESP-IDF tooling

### Build Steps

```bash
idf.py build
idf.py flash
idf.py monitor
```

## Notes

- Legal and regulatory compliance is your responsibility.  Verify legal status in your jurisdiction before use.

## Legal Disclaimer

This code is for **educational and research purposes only**. Users are responsible for ensuring their use complies with applicable laws and regulations in their jurisdiction.

Based on [AxonCadabra](https://github.com/WithLoveFromMinneapolis/AxonCadabra) for Android and [Axon_ON](https://github.com/KaraZajac/Axon_ON) for the Flipper Zero.

## License

See [LICENSE.md](LICENSE.md) for details.
