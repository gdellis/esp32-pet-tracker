"""SQLite database for storing tracker locations and device info."""

import sqlite3
from contextlib import contextmanager
from dataclasses import dataclass
from datetime import datetime
from typing import Generator, List, Optional


@dataclass
class Device:
    id: int
    device_id: int
    name: Optional[str]
    last_seen: datetime


@dataclass
class LocationRecord:
    id: int
    device_id: int
    latitude: float
    longitude: float
    altitude: float
    battery_pct: int
    valid_fix: bool
    raw_timestamp: int
    received_at: datetime

    def to_dict(self) -> dict:
        return {
            "id": self.id,
            "device_id": self.device_id,
            "latitude": self.latitude,
            "longitude": self.longitude,
            "altitude": self.altitude,
            "battery_pct": self.battery_pct,
            "valid_fix": self.valid_fix,
            "raw_timestamp": self.raw_timestamp,
            "received_at": self.received_at.isoformat(),
        }


class Database:
    def __init__(self, path: str = "tracker.db") -> None:
        self._path = path
        self._init_db()

    @contextmanager
    def _get_conn(self) -> Generator[sqlite3.Connection, None, None]:
        conn = sqlite3.connect(self._path)
        conn.row_factory = sqlite3.Row
        try:
            yield conn
            conn.commit()
        finally:
            conn.close()

    def _init_db(self) -> None:
        with self._get_conn() as conn:
            conn.executescript("""
                CREATE TABLE IF NOT EXISTS devices (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    device_id INTEGER UNIQUE NOT NULL,
                    name TEXT,
                    last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                );

                CREATE TABLE IF NOT EXISTS locations (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    device_id INTEGER NOT NULL,
                    latitude REAL NOT NULL,
                    longitude REAL NOT NULL,
                    altitude REAL NOT NULL,
                    battery_pct INTEGER NOT NULL,
                    valid_fix BOOLEAN NOT NULL,
                    raw_timestamp INTEGER NOT NULL,
                    received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    FOREIGN KEY (device_id) REFERENCES devices(device_id)
                );

                CREATE INDEX IF NOT EXISTS idx_locations_device_id
                    ON locations(device_id);
                CREATE INDEX IF NOT EXISTS idx_locations_received_at
                    ON locations(received_at);
            """)

    def upsert_device(self, device_id: int, name: Optional[str] = None) -> Device:
        with self._get_conn() as conn:
            conn.execute(
                """
                INSERT INTO devices (device_id, name, last_seen)
                VALUES (?, ?, CURRENT_TIMESTAMP)
                ON CONFLICT(device_id) DO UPDATE SET
                    last_seen = CURRENT_TIMESTAMP,
                    name = COALESCE(?, name)
                """,
                (device_id, name, name),
            )
            row = conn.execute(
                "SELECT * FROM devices WHERE device_id = ?",
                (device_id,),
            ).fetchone()
            return Device(
                id=row["id"],
                device_id=row["device_id"],
                name=row["name"],
                last_seen=datetime.fromisoformat(row["last_seen"]),
            )

    def insert_location(
        self,
        device_id: int,
        latitude: float,
        longitude: float,
        altitude: float,
        battery_pct: int,
        valid_fix: bool,
        raw_timestamp: int,
    ) -> LocationRecord:
        self.upsert_device(device_id)

        with self._get_conn() as conn:
            cursor = conn.execute(
                """
                INSERT INTO locations (
                    device_id, latitude, longitude, altitude,
                    battery_pct, valid_fix, raw_timestamp
                )
                VALUES (?, ?, ?, ?, ?, ?, ?)
                """,
                (
                    device_id,
                    latitude,
                    longitude,
                    altitude,
                    battery_pct,
                    valid_fix,
                    raw_timestamp,
                ),
            )
            row = conn.execute(
                "SELECT * FROM locations WHERE id = ?",
                (cursor.lastrowid,),
            ).fetchone()
            return LocationRecord(
                id=row["id"],
                device_id=row["device_id"],
                latitude=row["latitude"],
                longitude=row["longitude"],
                altitude=row["altitude"],
                battery_pct=row["battery_pct"],
                valid_fix=bool(row["valid_fix"]),
                raw_timestamp=row["raw_timestamp"],
                received_at=datetime.fromisoformat(row["received_at"]),
            )

    def get_devices(self) -> List[Device]:
        with self._get_conn() as conn:
            rows = conn.execute(
                "SELECT * FROM devices ORDER BY last_seen DESC"
            ).fetchall()
            return [
                Device(
                    id=row["id"],
                    device_id=row["device_id"],
                    name=row["name"],
                    last_seen=datetime.fromisoformat(row["last_seen"]),
                )
                for row in rows
            ]

    def get_device_locations(
        self,
        device_id: int,
        limit: int = 100,
    ) -> List[LocationRecord]:
        with self._get_conn() as conn:
            rows = conn.execute(
                """
                SELECT * FROM locations
                WHERE device_id = ?
                ORDER BY received_at DESC
                LIMIT ?
                """,
                (device_id, limit),
            ).fetchall()
            return [
                LocationRecord(
                    id=row["id"],
                    device_id=row["device_id"],
                    latitude=row["latitude"],
                    longitude=row["longitude"],
                    altitude=row["altitude"],
                    battery_pct=row["battery_pct"],
                    valid_fix=bool(row["valid_fix"]),
                    raw_timestamp=row["raw_timestamp"],
                    received_at=datetime.fromisoformat(row["received_at"]),
                )
                for row in rows
            ]

    def get_latest_location(self, device_id: int) -> Optional[LocationRecord]:
        locations = self.get_device_locations(device_id, limit=1)
        return locations[0] if locations else None

    def get_latest_location_for_all(self) -> dict[int, LocationRecord]:
        with self._get_conn() as conn:
            rows = conn.execute("""
                SELECT l.* FROM locations l
                INNER JOIN (
                    SELECT device_id, MAX(received_at) as max_ts
                    FROM locations
                    GROUP BY device_id
                ) latest ON l.device_id = latest.device_id AND l.received_at = latest.max_ts
            """).fetchall()
            return {
                row["device_id"]: LocationRecord(
                    id=row["id"],
                    device_id=row["device_id"],
                    latitude=row["latitude"],
                    longitude=row["longitude"],
                    altitude=row["altitude"],
                    battery_pct=row["battery_pct"],
                    valid_fix=bool(row["valid_fix"]),
                    raw_timestamp=row["raw_timestamp"],
                    received_at=datetime.fromisoformat(row["received_at"]),
                )
                for row in rows
            }

    def update_device_name(self, device_id: int, name: str) -> Optional[Device]:
        with self._get_conn() as conn:
            conn.execute(
                "UPDATE devices SET name = ? WHERE device_id = ?",
                (name, device_id),
            )
            row = conn.execute(
                "SELECT * FROM devices WHERE device_id = ?",
                (device_id,),
            ).fetchone()
            if row:
                return Device(
                    id=row["id"],
                    device_id=row["device_id"],
                    name=row["name"],
                    last_seen=datetime.fromisoformat(row["last_seen"]),
                )
            return None
