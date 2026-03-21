# AGENTS.md

This is a Rust project for ESP32 (pet tracker).

## Project Overview

- **Type**: Embedded Rust project (ESP32)
- **Stack**: Rust, ESP-IDF, LoRa SX1262, GPS, BLE
- **Reference**: https://docs.espressif.com/projects/rust/book/preface.html

## Pre-Commit Hooks

Run before every commit:

```bash
pre-commit run --all-files
```

This lints markdown files and checks for common issues. Fix any errors before committing.

## Build Commands

```bash
# Build for ESP32 target
cargo build --release --target xtensa-esp32-elf

# Build for ESP32-S3 (recommended for most ESP32-S3 boards)
cargo build --release --target xtensa-esp32s3-elf

# Flash to device (adjust /dev/ttyUSB0 as needed)
espflash flash /dev/ttyUSB0 --monitor

# Build with logging
cargo build --release -vv

# Release with ESP-specific optimizations
cargo build --release --target xtensa-esp32s3-elf --features esp32s3
```

## Code Style Guidelines

### Rust Conventions

- **Formatting**: Run `cargo fmt` before committing
- **Linting**: Run `cargo clippy --all-targets --all-features -- -D warnings`
- **Imports**: Use absolute paths with `use` statements at module top
- **Naming**:
  - Types/Structs: `PascalCase` (e.g., `PetTracker`, `LocationData`)
  - Functions/Variables: `snake_case` (e.g., `get_location`, `battery_level`)
  - Constants: `SCREAMING_SNAKE_CASE` (e.g., `MAX_RETRIES`)
  - Enums: `PascalCase` variants (e.g., `Status::Active`)
- **Line length**: Max 100 characters
- **Docstrings**: Use `///` for public API docs, `//!` for module-level docs
- **Error handling**: Use `Result<T, E>` for fallible operations; propagate with `?`
- **No unwrap/expect in library code**: Return `Result` or use `unwrap_or`/`unwrap_or_else`

### ESP32 Specific

- **Panics**: In embedded context, panics should be handled gracefully (often via `panic!` to reboot)
- **Interrupts**: Keep interrupt handlers minimal; defer heavy work to main task
- **Concurrency**: Use `critical-section` mutexes or `ESP-IDF` synchronization primitives
- **Memory**: Avoid dynamic allocation where possible; prefer stack or static buffers

### Error Handling Pattern

```rust
// Preferred: Return Result for functions that can fail
fn read_sensor() -> Result<SensorData, SensorError> {
    // ... implementation
}

// For unrecoverable errors in embedded (system panic + reboot)
fn fatal_error(message: &str) -> ! {
    log::error!("{}", message);
    esp_idf_hal::reset::restart();
}
```

### Module Structure

```
src/
├── main.rs           # Application entry point
├── lib.rs            # Library root (reusable components)
├── wifi.rs           # WiFi functionality
├── ble.rs            # Bluetooth Low Energy
├── sensors.rs        # Sensor reading
├── storage.rs        # NVS or flash storage
└── error.rs          # Central error types
```

## Testing

```bash
# Run all tests
cargo test

# Run tests for a specific module
cargo test sensor

# Run tests with output
cargo test -- --nocapture

# Run doc tests
cargo test --doc

# Run integration tests (tests/ directory)
cargo test --test integration
```

## Common Issues

- **ESP32 toolchain**: Ensure `xtensa-esp32-elf` target is installed: `rustup target add xtensa-esp32-elf`
- **espflash**: Install via `cargo install espflash`
- **WIFI/BLE concurrency**: ESP-IDF has specific requirements for WiFi + BLE coexistence
- **Deep sleep**: Handle wake-up sources properly in `entry` point

## Dependencies to Know

- `esp-idf-svc`: ESP-IDF service layer bindings
- `esp-idf-hal`: Hardware abstraction layer
- `esp-idf-macros`: Procedural macros for ESP
- `embedded-svc`: Embedded services trait definitions
- `anyhow` / `thiserror`: For error handling (app level, not embedded)

## Git Workflow

### Branch Strategy

- `main`: Production-ready code (currently contains design docs only)
- Feature work happens in branches: `feat/<feature-name>`, `fix/<issue-name>`

### Commit Rules

- **Atomic commits**: One logical change per commit
- **Imperative mood**: "Add feature" not "Added feature"
- **Max 72 chars** for subject line
- **Reference issues**: "Fix #123, add GPS parsing" when applicable

### Process

1. Create branch from `main`: `git checkout -b feat/my-feature`
2. Make changes, commit incrementally
3. Run pre-commit: `pre-commit run --all-files`
4. Push branch: `git push -u origin feat/my-feature`
5. Create PR via GitHub CLI: `gh pr create`
6. Address review feedback
7. Merge via "Squash and merge" on GitHub

### Never Do

- Never commit directly to `main`
- Never force push to `main` or shared branches
- Never commit secrets, API keys, or credentials
- Never amend or rebase commits already pushed to shared branches

## Pull Requests

### PR Creation

```bash
# Create PR from current branch
gh pr create --title "feat: add GPS parsing module" --body "## Summary

- Add NMEA parsing for GGA/RMC sentences
- Implement GPS driver trait

## Testing

- Test with NEO-6M module outdoors"
```

### PR Guidelines

- **Title format**: `feat:`, `fix:`, `docs:`, `refactor:` prefix
- **Description**: Explain *why* not just *what*
- **Screenshots**: For UI changes (not applicable here)
- **Testing**: Describe how changes were verified
- **Breaking changes**: Note if any APIs changed

### PR Review

- Address all comments before merging
- Use "Request changes" only for blocking issues
- Approve when ready to merge
- Squash merge preferred for clean history

## GitHub CLI

```bash
# Create PR
gh pr create --title "feat: add GPS module" --body "$(cat <<'EOF'
## Summary
- Add GPS parsing with NMEA support
- Implement GpsDriver trait
EOF
)"

# View PR status
gh pr status

# Check PR diff
gh pr diff 123

# Merge PR
gh pr merge 123 --squash --delete-branch

# Close PR
gh pr close 123
```
