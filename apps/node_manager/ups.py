from nut2 import PyNUTClient
import typing

from apps.node_manager.config import Config


class UPS:

	def __init__(self, config: Config) -> None:
		self.clients: typing.Dict[str, PyNUTClient] = {}
		for name, ups in config.ups().items():
			try:
				self.clients[name] = PyNUTClient(
				    host=ups.hostname,  # type: ignore
				    port=ups.port,
				    login=ups.login,
				    password=ups.password)
			except Exception as e:
				print(f"Cannot connect to UPS '{name}': {e}")

	def batteries(self) -> typing.Any:

		batteries = {}
		for name, client in self.clients.items():
			for ups in client.GetUPSNames():  # type: ignore
				batteries[f"{name}.{ups}"] = float(
				    client.GetUPSVars(ups=ups).get("battery.charge")) / 100.  # type: ignore

		return batteries if batteries else None
