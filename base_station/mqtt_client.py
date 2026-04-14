"""MQTT client for publishing tracker data to HiveMQ."""

import json
import logging
from datetime import datetime
from typing import Callable, Optional

import paho.mqtt.client as mqtt

from config import MqttConfig
from packet_parser import TrackerLocation


logger = logging.getLogger(__name__)


class MqttClient:
    def __init__(self, config: MqttConfig) -> None:
        self._config = config
        self._client: Optional[mqtt.Client] = None
        self._connected = False

    @property
    def is_connected(self) -> bool:
        return self._connected

    def connect(self) -> None:
        self._client = mqtt.Client()
        self._client.on_connect = self._on_connect
        self._client.on_disconnect = self._on_disconnect

        if self._config.username:
            self._client.username_pw_set(
                self._config.username,
                self._config.password,
            )

        self._client.connect(
            self._config.host,
            self._config.port,
            self._config.keepalive,
        )
        self._client.loop_start()

    def _on_connect(
        self,
        client: mqtt.Client,
        userdata: None,
        flags: dict,
        rc: int,
    ) -> None:
        if rc == 0:
            self._connected = True
            logger.info(
                "Connected to MQTT broker at %s:%d",
                self._config.host,
                self._config.port,
            )
        else:
            logger.error("MQTT connection failed with code %d", rc)

    def _on_disconnect(
        self,
        client: mqtt.Client,
        userdata: None,
        rc: int,
    ) -> None:
        self._connected = False
        if rc != 0:
            logger.warning("Disconnected from MQTT broker unexpectedly (code %d)", rc)

    def disconnect(self) -> None:
        if self._client:
            self._client.loop_stop()
            self._client.disconnect()
            self._connected = False

    def publish_location(self, location: TrackerLocation) -> bool:
        if not self._connected:
            logger.warning("Cannot publish: not connected to MQTT broker")
            return False

        topic = f"{self._config.topic_prefix}/device/{location.device_id}/location"
        payload = json.dumps(location.to_dict())

        result = self._client.publish(topic, payload)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            logger.debug("Published location to %s", topic)
            return True
        else:
            logger.error("Failed to publish: %s", mqtt.error_string(result.rc))
            return False

    def subscribe_locations(
        self,
        callback: Callable[[TrackerLocation], None],
        qos: int = 1,
    ) -> None:
        topic = f"{self._config.topic_prefix}/device/+/location"

        def on_message(
            client: mqtt.Client,
            userdata: None,
            msg: mqtt.MQTTMessage,
        ) -> None:
            try:
                data = json.loads(msg.payload.decode())
                location = TrackerLocation(
                    device_id=data["device_id"],
                    latitude=data["latitude"],
                    longitude=data["longitude"],
                    altitude=data["altitude"],
                    battery_pct=data["battery_pct"],
                    valid_fix=data["valid_fix"],
                    timestamp=datetime.fromisoformat(data["timestamp"]),
                    raw_timestamp=data["raw_timestamp"],
                )
                callback(location)
            except (json.JSONDecodeError, KeyError) as e:
                logger.error("Failed to parse location message: %s", e)

        self._client.subscribe(topic, qos)
        self._client.message_callback_add(topic, on_message)
