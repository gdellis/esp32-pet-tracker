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

# 2. Install ESP32S3 target (includes Rust toolchain + GCC + LLVM)
rm -rf ~/.espup
espup install -t esp32s3 --export-file ~/export-esp.sh

# 3. Source environment
. ~/export-esp.sh
```

### Build

```bash
. ~/export-esp.sh
cargo build --release
```

---

## Configuration Files

### rust-toolchain.toml

```toml
[toolchain]
channel = "esp"
```

### .cargo/config.toml

```toml
[target.xtensa-esp32s3-none-elf]
runner = "espflash flash --monitor --chip esp32s3"

[env]

[build]
rustflags = [
  "-C", "link-arg=-nostartfiles",
]

[unstable]
build-std = ["core"]
```

---

## Dependencies

| Crate | Version | Purpose |
|-------|---------|---------|
| `esp-hal` | 1.0 | Hardware abstraction layer |
| `esp-bootloader-esp-idf` | 0.4.0 | ESP-IDF bootloader |
| `critical-section` | 1.2.0 | Critical section management |
| `log` | 0.4 | Logging facade |

**Note**: `anyhow` and `thiserror` are NOT compatible with `no_std` builds.

---

## Build Profiles

```toml
[profile.dev]
opt-level = "s"

[profile.release]
codegen-units = 1
debug = 2
debug-assertions = false
incremental = false
lto = "fat"
opt-level = "s"
overflow-checks = false
```

---

## Troubleshooting

### SIGSEGV during build

If you get `SIGSEGV` errors when building in the project directory, build from `/tmp` instead:

```bash
cp -r /path/to/project/* /tmp/build-temp/
cd /tmp/build-temp
. ~/export-esp.sh
cargo build --release
```

### "duplicate lang item" errors

This usually means you're mixing `std` and `no_std` crates. Use only `no_std` compatible crates.

### "linker not found" errors

Make sure to source the export script:

```bash
. ~/export-esp.sh
```
