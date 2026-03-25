# Dependencies

## Required Software

### System Dependencies (Linux/Ubuntu)

```bash
sudo apt-get install -y gcc build-essential cmake python3 git
```

### ESP-IDF v6.0

#### Installation (Using Docker - Recommended)

The project uses Docker for consistent builds:

```bash
cd firmware
./build.sh
```

#### Manual Installation

```bash
# Clone ESP-IDF
git clone -b v6.0 --recursive https://github.com/espressif/esp-idf.git ~/.espressif/v6.0/esp-idf

# Install dependencies
~/.espressif/v6.0/esp-idf/install.sh

# Export environment
source ~/.espressif/v6.0/esp-idf/export.sh
```

---

## Build Commands

### Docker Build (Recommended)

```bash
cd firmware
./build.sh
```

### Manual Build

```bash
source ~/.espressif/v6.0/esp-idf/export.sh
cd firmware
idf.py build
```

### Flash to Device

```bash
source ~/.espressif/v6.0/esp-idf/export.sh
idf.py -p /dev/ttyACM0 flash monitor
```

---

## ESP-IDF Components Used

| Component | Version | Purpose |
|-----------|---------|---------|
| `driver` | v6.0 | GPIO, UART, SPI, I2C |
| `freertos` | v6.0 | Real-time OS |
| `esp_timer` | v6.0 | High-resolution timer |
| `esp_log` | v6.0 | Logging |
| `nvs_flash` | v6.0 | Non-volatile storage |
| `esp_bt` | v6.0 | Bluetooth (optional) |

---

## Testing Dependencies

### Host-Based Unit Tests

Tests run on Linux without hardware using mocked ESP-IDF headers.

```bash
cd firmware/tests
mkdir -p build && cd build
cmake .. && make -j$(nproc)
./test_nmea_parser    # NMEA parsing (43 assertions)
./test_button_handler # Button debounce (4 assertions)
```

### Catch2

Catch2 v3.4.0 is fetched via CMake FetchContent - no separate installation needed.

---

## Troubleshooting

### "No such file or directory: espressif/idf:v6.0"

Use the correct Docker image tag:

```bash
docker run --rm -it espressif/idf:v6.0
```

### "idf.py: command not found"

Source the ESP-IDF environment:

```bash
source ~/.espressif/v6.0/esp-idf/export.sh
```

### "Permission denied" on /dev/ttyACM0

Add user to dialout group:

```bash
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```
