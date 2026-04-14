"""Base Station Configuration"""

from dataclasses import dataclass


@dataclass
class MqttConfig:
    host: str = "localhost"
    port: int = 1883
    username: str = ""
    password: str = ""
    topic_prefix: str = "lorapaws"
    keepalive: int = 60


@dataclass
class LoraConfig:
    frequency_mhz: float = 915.0
    spreading_factor: int = 7
    bandwidth_hz: int = 125000
    coding_rate: int = 5
    preamble_length: int = 8
    tx_power_dbm: int = 22


@dataclass
class ServerConfig:
    host: str = "0.0.0.0"
    port: int = 5000
    debug: bool = False


@dataclass
class DatabaseConfig:
    path: str = "tracker.db"


@dataclass
class Config:
    mqtt: MqttConfig = MqttConfig()
    lora: LoraConfig = LoraConfig()
    server: ServerConfig = ServerConfig()
    database: DatabaseConfig = DatabaseConfig()


def load_config() -> Config:
    return Config()
