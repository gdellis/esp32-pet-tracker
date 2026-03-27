#!/usr/bin/env bash
set -e

# Install 1Password CLI
curl -sS https://downloads.1password.com/linux/keys/1password.asc | gpg --dearmor --output /usr/share/keyrings/1password-archive-keyring.gpg
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/1password-archive-keyring.gpg] https://downloads.1password.com/linux/debian/amd64 stable main" | tee /etc/apt/sources.list.d/1password.list
mkdir -p /etc/debsig/policies/AC2D62742012EA22
curl -sS https://downloads.1password.com/linux/debian/debsig/1password.pol | tee /etc/debsig/policies/AC2D62742012EA22/1password.pol
mkdir -p /usr/share/debsig/keyrings/AC2D62742012EA22
curl -sS https://downloads.1password.com/linux/keys/1password.asc | gpg --dearmor --output /usr/share/debsig/keyrings/AC2D62742012EA22/debsig.gpg
apt-get update
apt-get install -y 1password-cli
echo "1Password CLI installed. Run 'op signin' to authenticate."

# Install OpenCode CLI
curl -s https://raw.githubusercontent.com/opencode-ai/opencode-cli/main/install.sh | bash
echo "OpenCode CLI installed."

# Run ESP-IDF setup
bash scripts/get ESP_IDF || true