# AxonOnOnOn

Allows a BLE capable ESP32 device to wirelessly trigger Axon body and dash cameras to start recording.

## Features

- Continuously broadcasts the signal for Axon body and dask cameras to start recording.
- Each broadcast uses a randomized MAC address based on Axon's OUI prefix of `00:25:df`

## Notes

- Based on [AxonCadabra](https://github.com/WithLoveFromMinneapolis/AxonCadabra) for Android and [Axon_ON](https://github.com/KaraZajac/Axon_ON) for the Flipper Zero.
- Don't bet your life on this working - cameras can be configured to ignore non-allowlisted Axon serial numbers.
- Cameras can only be forced to START recording, the camera operator always has the ability to hit the action button on the camera and stop the recording.
- Once a given BLE device has triggered a given camera, there is some cool down before that device can trigger it again. The fuzz mode is an attempt to bypass this trigger cool down restriction.

## Building

You'll need:
- ESP32-C3,S3 or compatible ESP32 board
- ESP-IDF v5.x or later
- Basic familiarity with ESP-IDF tooling

### Build Steps

```bash
idf.py build
idf.py flash
idf.py monitor
```

## Legal Disclaimer

Users are responsible for ensuring their use complies with applicable laws and regulations in their jurisdiction.

## License

See [LICENSE.md](LICENSE.md) for details.
