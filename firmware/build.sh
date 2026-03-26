#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"

usage() {
	cat <<EOF
Usage: $0 [--board esp32s3|esp32c6]

Options:
    --board BOARD    Target board: esp32s3 (default) or esp32c6
    --flash          Flash after building
    --monitor        Open serial monitor after flashing
    -h, --help       Show this help

Examples:
    $0                      # Build for ESP32S3
    $0 --board esp32c6      # Build for ESP32C6
    $0 --flash --monitor    # Build, flash, and monitor
EOF
}

BOARD="esp32s3"
FLASH=false
MONITOR=false

while [[ $# -gt 0 ]]; do
	case $1 in
	--board)
		BOARD="$2"
		shift 2
		;;
	--flash)
		FLASH=true
		shift
		;;
	--monitor)
		MONITOR=true
		shift
		;;
	-h | --help)
		usage
		exit 0
		;;
	*)
		echo "Unknown option: $1"
		usage
		exit 1
		;;
	esac
done

case $BOARD in
esp32s3)
	TARGET="esp32s3"
	;;
esp32c6)
	TARGET="esp32c6"
	;;
*)
	echo "Invalid board: $BOARD"
	echo "Valid options: esp32s3, esp32c6"
	exit 1
	;;
esac

IDF_CMD="idf.py -D IDF_TARGET=${TARGET} build"
if [[ "$FLASH" == true ]]; then
	IDF_CMD="${IDF_CMD} flash"
fi
if [[ "$MONITOR" == true ]]; then
	IDF_CMD="${IDF_CMD} monitor"
fi

docker run --rm \
	-v "${PROJECT_DIR}:/workspace" \
	-w /workspace \
	-u "$(id -u):$(id -g)" \
	-e IDF_PATH=/opt/esp/idf \
	-e HOME=/tmp \
	espressif/idf:v6.0 \
	sh -c ". /opt/esp/idf/export.sh && ${IDF_CMD}"
