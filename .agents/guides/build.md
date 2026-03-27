# Build Guide

## Docker Build (Recommended)

```bash
cd firmware
./build.sh                    # Build for ESP32S3 (default)
./build.sh --board esp32c6    # Build for ESP32C6
./build.sh --flash --monitor  # Build, flash, and monitor
```

## Flash to Device

```bash
docker run --rm -v $(pwd):/workspace -w /workspace espressif/idf:v6.0 \
  sh -c ". /opt/esp/idf/export.sh && idf.py -D IDF_TARGET=esp32s3 -p /dev/ttyACM0 flash monitor"
```

## Manual Build

```bash
cd firmware
idf.py set-target esp32s3    # Set target once
idf.py build                  # Build
idf.py -p /dev/ttyACM0 flash monitor  # Flash and monitor
```

## CI Build Notes

The CI builds for **both ESP32S3 and ESP32C6 targets**:
- Always test locally for your target first
- If CI fails with target mismatch, run `rm -rf build sdkconfig` locally and rebuild
- When adding ESP-IDF APIs, verify the API exists in ESP-IDF v6.0 for both architectures
