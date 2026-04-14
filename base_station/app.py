"""Flask web application for tracker monitoring."""

import logging

from flask import Flask, jsonify, render_template, request

from config import Config, load_config
from database import Database
from mqtt_client import MqttClient
from packet_parser import TrackerLocation


logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
)
logger = logging.getLogger(__name__)


def create_app(config: Config) -> Flask:
    app = Flask(__name__)
    app.config["DEBUG"] = config.server.debug

    db = Database(config.database.path)
    mqtt = MqttClient(config.mqtt)

    @app.route("/")
    def index():
        devices = db.get_devices()
        return render_template("index.html", devices=devices)

    @app.route("/api/devices")
    def get_devices():
        devices = db.get_devices()
        return jsonify(
            [
                {
                    "id": d.id,
                    "device_id": d.device_id,
                    "name": d.name,
                    "last_seen": d.last_seen.isoformat(),
                }
                for d in devices
            ]
        )

    @app.route("/api/devices/<int:device_id>")
    def get_device(device_id: int):
        locations = db.get_device_locations(device_id, limit=100)
        latest = db.get_latest_location(device_id)
        return jsonify(
            {
                "device": {
                    "id": latest.device_id if latest else device_id,
                    "device_id": device_id,
                    "latest": latest.to_dict() if latest else None,
                },
                "locations": [loc.to_dict() for loc in locations],
            }
        )

    @app.route("/api/devices/<int:device_id>/locations")
    def get_device_locations(device_id: int):
        limit = request.args.get("limit", 100, type=int)
        locations = db.get_device_locations(device_id, limit=min(limit, 1000))
        return jsonify([loc.to_dict() for loc in locations])

    @app.route("/api/devices/<int:device_id>/name", methods=["POST"])
    def update_device_name(device_id: int):
        data = request.get_json()
        name = data.get("name", "").strip() or None
        device = db.update_device_name(device_id, name or f"Device {device_id}")
        if device:
            return jsonify({"success": True, "device": device.name})
        return jsonify({"success": False, "error": "Device not found"}), 404

    def on_location(location: TrackerLocation) -> None:
        logger.info(
            "Received location from device %d: %.6f, %.6f (battery %d%%)",
            location.device_id,
            location.latitude,
            location.longitude,
            location.battery_pct,
        )
        db.insert_location(
            device_id=location.device_id,
            latitude=location.latitude,
            longitude=location.longitude,
            altitude=location.altitude,
            battery_pct=location.battery_pct,
            valid_fix=location.valid_fix,
            raw_timestamp=location.raw_timestamp,
        )
        mqtt.publish_location(location)

    @app.before_request
    def connect_mqtt():
        if not mqtt.is_connected:
            try:
                mqtt.connect()
                mqtt.subscribe_locations(on_location)
                logger.info("MQTT connected and subscribed")
            except Exception as e:
                logger.warning("Failed to connect to MQTT: %s", e)

    @app.teardown_appcontext
    def disconnect_mqtt(exception):
        pass

    return app


if __name__ == "__main__":
    config = load_config()
    app = create_app(config)
    app.run(
        host=config.server.host,
        port=config.server.port,
        debug=config.server.debug,
    )
