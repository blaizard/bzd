import argparse
import json
import time
import threading
import typing
import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion


class Dyson:
	def __init__(self, serial: str, password: str, ip: str, type: str, intervalS: int) -> None:
		self.serial = serial
		self.password = password
		self.ip = ip
		self.type = type
		self.intervalS = intervalS
		self.client: typing.Optional[mqtt.Client] = None

	def kelvin_to_celsius(self, kelvin_x10: int) -> typing.Optional[float]:
		try:
			return round((float(kelvin_x10) / 10.0) - 273.15, 1)
		except (ValueError, TypeError):
			return None

	def safe_int(self, val: str) -> typing.Optional[int]:
		try:
			return int(val)
		except (ValueError, TypeError):
			return None

	def send_state_request(self) -> None:
		"""Sends the status request command to the local Dyson broker."""
		request_msg = {"msg": "REQUEST-CURRENT-STATE", "time": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}
		assert self.client is not None
		self.client.publish(f"{self.type}/{self.serial}/command", json.dumps(request_msg))

	def background_poll_loop(self) -> None:
		"""Background worker that continuously polls the Dyson on a timer."""
		while True:
			time.sleep(self.intervalS)
			self.send_state_request()

	def start(self) -> None:
		self.client = mqtt.Client(callback_api_version=CallbackAPIVersion.VERSION2, client_id="", clean_session=True)
		self.client.username_pw_set(username=self.serial, password=self.password)

		self.client.on_connect = self.on_connect
		self.client.on_subscribe = self.on_subscribe
		self.client.on_message = self.on_message

		print(f"Connecting to device at {self.ip}...")
		self.client.connect(self.ip, 1883, 60)
		self.client.loop_forever()

	def on_connect(
		self,
		client: mqtt.Client,
		userdata: typing.Any,
		flags: typing.Any,
		reason_code: typing.Any,
		properties: typing.Any = None,
	) -> None:
		if reason_code == 0:
			print(f"Connected to Dyson {self.type} at {self.ip}!")
			client.subscribe(f"{self.type}/{self.serial}/status/current")
		else:
			print(f"Connection failed with reason code: {reason_code}")

	def on_subscribe(
		self,
		client: mqtt.Client,
		userdata: typing.Any,
		mid: typing.Any,
		reason_code_list: typing.Any,
		properties: typing.Any = None,
	) -> None:
		# Send initial request immediately upon connection
		self.send_state_request()

		# Start thread for periodic background requests
		if not hasattr(client, "polling_started"):
			client.polling_started = True  # type: ignore
			poll_thread = threading.Thread(target=self.background_poll_loop, daemon=True)
			poll_thread.start()

	def on_message(self, client: mqtt.Client, userdata: typing.Any, msg: typing.Any) -> None:
		try:
			payload = json.loads(msg.payload.decode("utf-8"))
			msg_type = payload.get("msg")

			if msg_type == "ENVIRONMENTAL-CURRENT-SENSOR-DATA":
				data = payload.get("data", {})

				temp_k = self.safe_int(data.get("tact"))
				humidity = self.safe_int(data.get("hact"))
				pm25 = self.safe_int(data.get("pm25"))
				pm10 = self.safe_int(data.get("pm10"))
				voc = self.safe_int(data.get("va10"))
				no2 = self.safe_int(data.get("noxl"))

				timestamp = time.strftime("%H:%M:%S")
				print(f"\n[{timestamp}] --- Sensor Readings ---")
				if temp_k is not None:
					print(f"Temperature : {self.kelvin_to_celsius(temp_k)} °C")
				if humidity is not None:
					print(f"Humidity    : {humidity}%")
				if pm25 is not None:
					print(f"PM 2.5      : {pm25} µg/m³")
				if pm10 is not None:
					print(f"PM 10       : {pm10} µg/m³")
				if voc is not None:
					print(f"VOC Index   : {voc}")
				if no2 is not None:
					print(f"NO2 Index   : {no2}")

		except Exception as e:
			print(f"Error parsing message: {e}")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Dyson sensor fetcher.")
	parser.add_argument("--ip", required=True, help="Device IP.")
	parser.add_argument(
		"--serial",
		required=True,
		help="The serial number.",
	)
	parser.add_argument(
		"--password",
		required=True,
		help="The MQTT password.",
	)
	parser.add_argument(
		"--type",
		required=True,
		help="The product type.",
	)
	parser.add_argument(
		"--interval",
		default=30,
		type=int,
		help="The polling interval in seconds.",
	)
	args = parser.parse_args()

	dyson = Dyson(
		serial=args.serial,
		password=args.password,
		ip=args.ip,
		type=args.type,
		intervalS=args.interval,
	)
	dyson.start()
