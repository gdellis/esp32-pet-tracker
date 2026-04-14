let currentDeviceId = null;
let devices = [];

async function loadDevices() {
    try {
        const response = await fetch('/api/devices');
        devices = await response.json();
        renderDevices();
    } catch (error) {
        console.error('Failed to load devices:', error);
        document.getElementById('devices').innerHTML =
            '<p class="loading">Failed to load devices</p>';
    }
}

function renderDevices() {
    const container = document.getElementById('devices');
    if (devices.length === 0) {
        container.innerHTML = '<p class="loading">No devices registered</p>';
        return;
    }

    container.innerHTML = devices.map(device => `
        <div class="device-card" onclick="selectDevice(${device.device_id})">
            <div class="name">${device.name || `Device ${device.device_id}`}</div>
            <div class="stats">
                <span class="battery">--</span>
                <span class="last-seen">${formatTime(device.last_seen)}</span>
            </div>
        </div>
    `).join('');
}

async function selectDevice(deviceId) {
    currentDeviceId = deviceId;
    try {
        const response = await fetch(`/api/devices/${deviceId}`);
        const data = await response.json();

        document.getElementById('device-detail').style.display = 'block';
        document.getElementById('device-name').textContent =
            data.device.name || `Device ${deviceId}`;

        if (data.latest) {
            document.getElementById('battery').textContent =
                `${data.latest.battery_pct}%`;
            document.getElementById('battery').className = 'value ' + getBatteryClass(data.latest.battery_pct);
            document.getElementById('location').textContent =
                `${data.latest.latitude.toFixed(6)}, ${data.latest.longitude.toFixed(6)}`;
            document.getElementById('last-update').textContent =
                formatTime(data.latest.received_at);
            document.getElementById('signal').textContent =
                data.latest.valid_fix ? 'Valid' : 'Invalid';
        }

        const tbody = document.getElementById('history-body');
        tbody.innerHTML = data.locations.map(loc => `
            <tr>
                <td>${formatTime(loc.received_at)}</td>
                <td>${loc.latitude.toFixed(6)}</td>
                <td>${loc.longitude.toFixed(6)}</td>
                <td>${loc.altitude.toFixed(1)}m</td>
                <td>${loc.battery_pct}%</td>
                <td class="${loc.valid_fix ? 'valid' : 'invalid'}">
                    ${loc.valid_fix ? 'Yes' : 'No'}
                </td>
            </tr>
        `).join('');
    } catch (error) {
        console.error('Failed to load device:', error);
    }
}

function getBatteryClass(pct) {
    if (pct <= 20) return 'low';
    if (pct <= 50) return 'medium';
    return '';
}

function formatTime(isoString) {
    const date = new Date(isoString);
    const now = new Date();
    const diff = now - date;

    if (diff < 60000) {
        return 'Just now';
    }
    if (diff < 3600000) {
        const mins = Math.floor(diff / 60000);
        return `${mins}m ago`;
    }
    if (diff < 86400000) {
        const hours = Math.floor(diff / 3600000);
        return `${hours}h ago`;
    }
    return date.toLocaleDateString();
}

setInterval(loadDevices, 30000);

document.addEventListener('DOMContentLoaded', loadDevices);
