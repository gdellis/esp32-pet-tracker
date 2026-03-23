#!/bin/bash
# ESP32 Tracker - Setup Script
# Installs required dependencies for building the firmware

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXPORT_FILE="$SCRIPT_DIR/esp32-export.sh"

echo "=== ESP32 Tracker Setup ==="

# Check for required system packages
check_system_deps() {
	echo "Checking system dependencies..."
	for pkg in gcc build-essential curl pkg-config; do
		if ! command -v "$pkg" &>/dev/null 2>&1; then
			echo "Warning: $pkg not found, attempting to install..."
			sudo apt-get install -y "$pkg" 2>/dev/null || true
		fi
	done
}

# Install espup if not present
install_espup() {
	if ! command -v espup &>/dev/null 2>&1; then
		echo "Installing espup..."
		cargo install espup
	else
		echo "espup already installed"
	fi
}

# Install ESP toolchain
install_toolchain() {
	echo "Installing ESP toolchain for ESP32S3..."
	rm -rf ~/.espup
	espup install -t esp32s3 --export-file "$EXPORT_FILE"
	echo "Toolchain installed"
}

# Main
check_system_deps
install_espup
install_toolchain

echo ""
echo "=== Setup Complete ==="
echo ""
echo "To build the project, run:"
echo "  source $EXPORT_FILE"
echo "  cargo build --release --target xtensa-esp32s3-espidf"
echo ""
echo "Or use the convenience alias:"
echo "  alias build-esp32='source $EXPORT_FILE && cargo build --release --target xtensa-esp32s3-espidf'"
echo ""
echo "Add this to your ~/.bashrc to persist the alias:"
echo "  echo \"alias build-esp32='source $EXPORT_FILE && cargo build --release --target xtensa-esp32s3-espidf'\" >> ~/.bashrc"
