#!/bin/bash
# Build script using ESP-IDF Docker container

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"

docker run --rm \
	-v "${PROJECT_DIR}:/workspace" \
	-w /workspace \
	-u "$(id -u):$(id -g)" \
	-e IDF_PATH=/opt/esp/idf \
	-e HOME=/tmp \
	espressif/idf:v5.3.1 \
	sh -c ". /opt/esp/idf/export.sh && idf.py build"
