import typing
import json

from bzd.utils.run import localCommand


class XSensors:

	def available(self) -> bool:
		result = localCommand(["which", "sensors"], ignoreFailure=True)
		return result.isSuccess()

	def get(self) -> typing.Any:
		raw = localCommand(["sensors", "-j"])
		xsensors = json.loads(raw.getStdout())

		data = {}
		for name, group in xsensors.items():
			groupData = []
			for cat, values in group.items():
				if isinstance(values, dict):
					for key, value in values.items():
						if key.startswith("temp") and key.endswith("input"):
							groupData.append(value)
			if groupData:
				data[name] = groupData

		return {"temperatures": data}
