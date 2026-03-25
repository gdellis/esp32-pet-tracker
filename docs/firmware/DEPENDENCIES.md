# Dependencies

## Required Software

### System Dependencies (Linux/Ubuntu)

```bash
sudo apt-get install -y gcc build-essential curl pkg-config
```

### Rust & ESP Toolchain

#### Installation

```bash
# 1. Install espup
cargo install espup

# 2. Install ESP32S3 target
rm -rf ~/.espup
espup install -t esp32s3 --export-file ~/export-esp.sh

# 3. Source environment
. ~/export-esp.sh
```

### Target Triple

Use `xtensa-esp32s3-espidf` for ESP32S3 with ESP-IDF.

---

## Build Commands

### Standard Build (requires target pre-installed)

```bash
. ~/export-esp.sh
cargo +esp build --release --target xtensa-esp32s3-espidf
```

### Build from Source (builds std library for target)

```bash
. ~/export-esp.sh
cargo +esp build --release --target xtensa-esp32s3-espidf -Zbuild-std=std
```

---

## Rust Crates

| Crate | Version | Purpose |
|-------|---------|---------|
| `esp-idf-svc` | 0.52 | ESP-IDF service layer (WiFi, BLE, etc.) |
| `esp-idf-hal` | 0.46 | Hardware abstraction layer |
| `esp-idf-sys` | 0.37 | ESP-IDF bindings |
| `nmea0183` | 0.6 | NMEA 0183 GPS parsing |
| `lis3dh` | 0.4 | LIS3DH accelerometer driver |
| `lora-phy` | 3 | LoRa PHY layer (supports SX1262) |
| `anyhow` | 1.0 | Error handling |
| `log` | 0.4 | Logging facade |
| `thiserror` | 1.0 | Error derive macros |

---

## Verified Cargo.toml

```toml
[package]
name = "esp32-tracker"
version = "0.1.0"
edition = "2021"

[dependencies]
esp-idf-svc = "0.52"
esp-idf-hal = "0.46"
esp-idf-sys = "0.37"
nmea0183 = "0.6"
lis3dh = "0.4"
lora-phy = "3"
anyhow = "1.0"
log = "0.4"
thiserror = "1.0"

[build-dependencies]

[profile.release]
opt-level = "s"
lto = true
```

---

## Troubleshooting

### "can't find crate for `core`"

The target isn't installed. Use the `-Zbuild-std=std` flag to build from source:

```bash
cargo +esp build --release --target xtensa-esp32s3-espidf -Zbuild-std=std
```

### Build hangs on "Compiling core"

Let it run - the first build from source takes 10+ minutes as it compiles the entire Rust standard library for the Xtensa target.

### "toolchain 'esp' does not support components"

This is expected. The espup toolchain doesn't support `rustup target add`. Use `-Zbuild-std=std` instead.

### "failed to run espup install"

Clean the espup cache:

```bash
rm -rf ~/.espup
espup install -t esp32s3
```
