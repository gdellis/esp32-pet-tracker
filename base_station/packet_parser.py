"""LoRa packet parser for tracker data.

Packet format (23 bytes):
    - bytes[0:4]: device_id (uint32_t, little-endian)
    - bytes[4:8]: latitude (int32_t, little-endian, degrees * 1e6)
    - bytes[8:12]: longitude (int32_t, little-endian, degrees * 1e6)
    - bytes[12:16]: altitude (int32_t, little-endian, meters * 100)
    - bytes[16:18]: battery (uint16_t, little-endian, percentage)
    - bytes[18]: flags (uint8_t, bit 0 = valid fix)
    - bytes[19:23]: timestamp (uint32_t, little-endian, seconds since boot)
"""

import struct
from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Optional


@dataclass
class TrackerLocation:
    device_id: int
    latitude: float  # degrees
    longitude: float  # degrees
    altitude: float  # meters
    battery_pct: int
    valid_fix: bool
    timestamp: datetime
    raw_timestamp: int

    @classmethod
    def from_packet(
        cls, packet: bytes, received_at: Optional[datetime] = None
    ) -> "TrackerLocation":
        if len(packet) < 23:
            raise ValueError(f"Packet too short: {len(packet)} bytes, expected 23")

        device_id, lat_raw, lon_raw, alt_raw = struct.unpack("<Iiii", packet[0:16])
        battery_raw, flags, ts_raw = struct.unpack("<HBI", packet[16:23])

        lat_deg = lat_raw / 1_000_000
        lon_deg = lon_raw / 1_000_000
        alt_m = alt_raw / 100
        valid = bool(flags & 0x01)

        if received_at is None:
            received_at = datetime.now(timezone.utc)

        ts_dt = datetime.fromtimestamp(ts_raw, tz=timezone.utc)

        return cls(
            device_id=device_id,
            latitude=lat_deg,
            longitude=lon_deg,
            altitude=alt_m,
            battery_pct=battery_raw,
            valid_fix=valid,
            timestamp=ts_dt,
            raw_timestamp=ts_raw,
        )

    def to_dict(self) -> dict:
        return {
            "device_id": self.device_id,
            "latitude": self.latitude,
            "longitude": self.longitude,
            "altitude": self.altitude,
            "battery_pct": self.battery_pct,
            "valid_fix": self.valid_fix,
            "timestamp": self.timestamp.isoformat(),
            "raw_timestamp": self.raw_timestamp,
        }


def parse_packet(
    packet: bytes, received_at: Optional[datetime] = None
) -> TrackerLocation:
    return TrackerLocation.from_packet(packet, received_at)
