# LoRaPaws32 Repository Reorganization

**Date:** April 2026
**Status:** Complete

## Overview

The LoRaPaws32 project was reorganized from a single monorepo into four separate repositories. This document describes what was done, why, and how to work with the new structure.

## Motivation

The original monorepo contained distinct components that had different:
- **Languages and frameworks** (C++/ESP-IDF, Python/Flask, KiCad)
- **Build systems** (idf.py, CMake, KiCad)
- **Release cycles** (firmware v1.2 while hardware is v2.0)
- **Community boundaries** (one component can be open-sourced without exposing everything)

Separating into individual repositories provides:
- Organizational clarity
- Independent versioning
- Focused CI/CD pipelines
- Easier contribution workflows
- Cleaner project management

## Original Structure

```
LoRaPaws32/ (monorepo)
├── firmware/              # ESP-IDF C++ (ESP32-S3/C6)
├── hardware/              # KiCad PCB designs
├── base_station/          # Python Flask web app
├── docs/                  # Markdown documentation
├── .github/workflows/     # CI/CD
└── LICENSE               # "Personal Use Only"
```

## New Structure

| Repository | URL | Purpose | License |
|------------|-----|---------|---------|
| `lorapaws32-firmware` | https://github.com/gdellis/lorapaws32-firmware | ESP-IDF C++ firmware | MIT |
| `lorapaws32-hardware` | https://github.com/gdellis/lorapaws32-hardware | KiCad PCB designs | CC BY-NC-SA 4.0 |
| `lorapaws32-base-station` | https://github.com/gdellis/lorapaws32-base-station | Python Flask web app | MIT |
| `LoRaPaws32` | https://github.com/gdellis/LoRaPaws32 | Meta-repo (docs, contracts) | CC BY-NC-SA 4.0 |

### Repository Details

#### lorapaws32-firmware

ESP-IDF C++ firmware for the pet tracker.

**Contents:**
- Complete ESP-IDF v6.0 project
- Host-based unit tests (CMake + Catch2)
- Driver code for GPS, LoRa, BLE, accelerometer
- State machine for tracker lifecycle

**Key Files:**
- `main/board_config.h` - Pin definitions (source of truth)
- `main/state_machine.cpp` - Tracker state machine
- `docs/PROTOCOL.md` - LoRa packet format

#### lorapaws32-hardware

KiCad PCB designs and enclosure files.

**Contents:**
- `tracker/` - Pet tracker PCB (ESP32 + LoRa + GPS)
- `base_station/` - Raspberry Pi HAT design
- `enclosure/` - 3D printable case designs

**Key Files:**
- `tracker/tracker.kicad_pcb` - Main tracker PCB
- `hardware/PINOUT.md` - Pin assignments (in meta-repo)

#### lorapaws32-base-station

Python Flask web application for the base station.

**Contents:**
- Flask web server with real-time tracking UI
- MQTT client for receiving LoRa data
- SQLite database for persistence
- REST API for device configuration

**Key Files:**
- `app.py` - Main Flask application
- `packet_parser.py` - LoRa packet decoder
- `docs/PROTOCOL.md` - Packet format reference

#### LoRaPaws32 (meta-repo)

Documentation and contract documents.

**Contents:**
- `docs/PROTOCOL.md` - Binary protocol specification
- `docs/hardware/PINOUT.md` - Pin assignments
- `clone-all.sh` - Script to clone all repos

## Contract Documents

Since components are now separate, contracts define the interfaces between them.

### PROTOCOL.md

Binary format for LoRa packets transmitted between tracker and base station.

**Packet Structure (23 bytes):**
```
Bytes 0-3:   device_id (uint32_t)
Bytes 4-7:   latitude * 1e6 (int32_t)
Bytes 8-11:  longitude * 1e6 (int32_t)
Bytes 12-15: altitude * 100 (int32_t)
Bytes 16-17: battery percentage (uint8_t)
Byte 18:     flags (0x01 = valid fix)
Bytes 19-22: timestamp (uint32_t)
```

**Endianness:** Big-endian

### PINOUT.md

Pin assignments for both ESP32-S3 and ESP32-C6 targets.

**Key pins:**
- GPS UART: TX/RX on GPIO7/GPIO15 (ESP32-S3)
- LoRa SPI: MOSI/MISO/SCK on GPIO4/5/6
- LED: GPIO40 (active high)
- Button: GPIO9 (active low)

## Migration Process

### Step 1: Backup

```bash
# Create bare clone backup
git clone --bare git@github.com:gdellis/LoRaPaws32.git \
  /media/glenn/GDE-SSD/repo-backups/lorapaws32-backup-20260418.git

# Create GitHub release
gh release create v0-pre-reorg \
  --title "Pre-reorganization snapshot" \
  --notes "Snapshot before splitting into multiple repos"

# Export issues and PRs
gh issue list --state all --json number,title,body > issues-pre-reorg.json
gh pr list --state all --json number,title,body > prs-pre-reorg.json
```

### Step 2: Extract Components with History

Used `git subtree split` to extract each component with full history:

```bash
# Extract firmware
git subtree split --prefix=firmware --branch=firmware-split
git push git@github.com:gdellis/lorapaws32-firmware.git firmware-split:main

# Extract hardware
git subtree split --prefix=hardware --branch=hardware-split
git push git@github.com:gdellis/lorapaws32-hardware.git hardware-split:main

# Extract base_station
git subtree split --prefix=base_station --branch=base-station-split
git push git@github.com:gdellis/lorapaws32-base-station.git base-station-split:main
```

### Step 3: Update Each Repository

For each new repo:
1. Add `README.md` with overview and links to sibling repos
2. Add appropriate `LICENSE` file
3. Add `AGENTS.md` with agent guidelines
4. Add `.agents/` directory with guides and rules
5. Add CI/CD workflows

### Step 4: Clean Up Meta-Repo

```bash
# Remove extracted directories from meta-repo
rm -rf firmware/ hardware/ base_station/

# Update README.md to reference sibling repos
# Update CHANGELOG.md

git commit -m "chore: remove components to convert to meta-repo"
git push
```

## Working with the New Structure

### Cloning All Repos

```bash
# Clone meta-repo
git clone git@github.com:gdellis/LoRaPaws32.git

# Use the clone script
cd LoRaPaws32
bash clone-all.sh
```

This creates:
```
LoRaPaws32/
├── lorapaws32-firmware/
├── lorapaws32-hardware/
└── lorapaws32-base-station/
```

### Development Workflow

Each repository is independent:

**Firmware:**
```bash
cd lorapaws32-firmware
. ~/.espressif/v6.0/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

**Base Station:**
```bash
cd lorapaws32-base-station
pip install -r requirements.txt
python app.py
```

**Hardware:**
Open `lorapaws32-hardware/tracker/tracker.kicad_pcb` in KiCad 8.0+.

### Synchronization

Since components evolve together, coordinate releases:

| Firmware | Hardware | Base Station | Notes |
|----------|----------|--------------|-------|
| v1.0.0 | v1.0.0 | v1.0.0 | Initial release |
| v1.1.0 | v1.0.0 | v1.1.0 | Firmware + base station update |
| v1.2.0 | v2.0.0 | v1.2.0 | Hardware redesign |

## What Was Preserved

- **Git history** - All commit history preserved via `git subtree split`
- **Issues and PRs** - Exported before migration
- **All branches** - Can be found in individual repo backups
- **Backups** - Full bare clone at `/media/glenn/GDE-SSD/repo-backups/`

## Rollback Plan

If rollback is needed:

1. **Restore from backup:**
   ```bash
   git clone /media/glenn/GDE-SSD/repo-backups/lorapaws32-backup-20260418.git
   ```

2. **Transfer repos back to personal account (if using organization)**
   ```bash
   gh repo transfer lorapaws32/firmware gdellis
   ```

## Known Limitations

### GitHub Projects (New)

GitHub Projects does NOT provide repo hierarchy like GitLab subgroups. It's a standalone project board that can link to issues/PRs across repos, but:
- No shared permissions
- No unified issue tracker
- Each repo still has its own issues/PRs

For this project, the GitHub Project at https://github.com/users/gdellis/projects/4 serves as a cross-repo tracker, but true hierarchy would require a GitHub Organization.

### Alternative Considered: GitHub Organization

A GitHub Organization would provide:
- Unified namespace (`lorapaws32/firmware`, etc.)
- Shared team permissions
- Single dashboard for all repos

However, for a personal project, the current setup with 4 repos under `gdellis/` is sufficient.

## File Inventory

### Meta-Repo (LoRaPaws32)

```
LoRaPaws32/
├── docs/
│   ├── PROTOCOL.md           # Binary protocol specification
│   └── hardware/
│       └── PINOUT.md         # Pin assignments
├── .agents/                  # Agent guidelines
│   ├── guides/
│   │   ├── build.md
│   │   ├── documentation.md
│   │   ├── ide-setup.md
│   │   └── testing.md
│   └── rules/
│       ├── git.md
│       ├── github.md
│       ├── markdown.md
│       ├── rust.md
│       └── workflow.md
├── .github/                  # GitHub workflows
├── AGENTS.md                 # Agent instructions
├── CHANGELOG.md             # Release notes
├── LICENSE                  # MIT
├── LICENSE-DESIGNS          # CC BY-NC-SA 4.0
├── README.md                # Overview
└── clone-all.sh           # Clone script
```

### Firmware Repo

```
lorapaws32-firmware/
├── main/                    # Source code
│   ├── main.cpp
│   ├── board_config.h
│   ├── state_machine.cpp
│   └── ...
├── tests/                   # Host-based tests
├── .agents/                 # Agent guidelines
├── .github/workflows/      # CI/CD
├── AGENTS.md
├── LICENSE                 # MIT
└── README.md
```

### Hardware Repo

```
lorapaws32-hardware/
├── tracker/                # Tracker PCB
├── base_station/           # Base station PCB
├── enclosure/              # 3D printed case
├── .agents/               # Agent guidelines
├── AGENTS.md
├── LICENSE-DESIGNS         # CC BY-NC-SA 4.0
└── README.md
```

### Base Station Repo

```
lorapaws32-base-station/
├── app.py                  # Flask application
├── packet_parser.py        # LoRa packet decoder
├── mqtt_client.py
├── database.py
├── config.py
├── .agents/               # Agent guidelines
├── .github/workflows/     # CI/CD
├── AGENTS.md
├── LICENSE                # MIT
└── README.md
```

## Conclusion

The repository reorganization is complete. Each component now lives in its own repository with:
- Independent versioning
- Focused CI/CD
- Clear contracts via shared documents
- Agent guidelines for development

The meta-repo provides a single entry point with documentation and contracts, while the clone script makes it easy to get all repos at once.
