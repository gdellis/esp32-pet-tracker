"""Tests for base station modules."""

import struct
import sys
import os

sys.path.insert(0, os.path.dirname(__file__))

from packet_parser import TrackerLocation, parse_packet


def test_parse_valid_packet():
    packet = struct.pack(
        "<IiiiHBI",
        0x12345678,
        40712800,
        -74006000,
        15000,
        85,
        0x01,
        1609459200,
    )
    location = parse_packet(packet)
    assert location.device_id == 0x12345678
    assert abs(location.latitude - 40.7128) < 0.0001
    assert abs(location.longitude - (-74.006)) < 0.0001
    assert abs(location.altitude - 150.0) < 0.001
    assert location.battery_pct == 85
    assert location.valid_fix is True
    print("test_parse_valid_packet: PASSED")


def test_parse_invalid_packet():
    try:
        parse_packet(b"too short")
        assert False, "Should have raised ValueError"
    except ValueError as e:
        assert "too short" in str(e)
        print("test_parse_invalid_packet: PASSED")


def test_location_to_dict():
    from datetime import datetime, timezone

    location = TrackerLocation(
        device_id=1,
        latitude=40.7128,
        longitude=-74.0060,
        altitude=10.0,
        battery_pct=75,
        valid_fix=True,
        timestamp=datetime(2024, 1, 1, 12, 0, 0, tzinfo=timezone.utc),
        raw_timestamp=1609459200,
    )
    d = location.to_dict()
    assert d["device_id"] == 1
    assert d["latitude"] == 40.7128
    assert d["valid_fix"] is True
    print("test_location_to_dict: PASSED")


if __name__ == "__main__":
    test_parse_valid_packet()
    test_parse_invalid_packet()
    test_location_to_dict()
    print("\nAll tests passed!")
