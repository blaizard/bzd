import pathlib
import json

from apps.trader_python.recording.recording import OHLC, Recording
from apps.trader_python.recording.recording_from_path import RecordingPairFromPath
from apps.trader_python.recording.recording_from_data import RecordingPairFromData


def _iterateByGroup(iterator, group) -> None:
	"""Iterate the recording by consecutive groups."""

	try:
		ohlc = next(iterator)
	except StopIteration:
		return

	context = {"iterator": iterator, "ohlc": ohlc, "continue": True}

	while context["continue"]:

		def groupIterator(current, context) -> OHLC:
			while True:
				yield context["ohlc"]
				try:
					context["ohlc"] = next(context["iterator"])
				except StopIteration:
					context["continue"] = False
					break
				if current != group(context["ohlc"]):
					break

		current = group(context["ohlc"])
		yield current, groupIterator(current, context)


def recordingToPath(recording: Recording, path: pathlib.Path) -> None:
	"""Save a recording to a specific path."""

	resolvedPath = path.expanduser().resolve()

	for pair in recording:
		current = resolvedPath / pair.uid.replace("/", "-")
		current.mkdir(parents=True, exist_ok=True)

		# Save the info
		info = json.dumps(pair.info.toDict(), indent=4)
		(current / "info.json").write_text(info)

		# Save the OHLC prices
		for key, iterator in _iterateByGroup(pair.__iter__(), group=lambda x: x.date.strftime("%Y-%m")):
			prices = RecordingPairFromData.fromPrices(iterator, first="x", second="y")
			filePath = current / f"{key}.csv"

			# Merge entries with the existing one if any.
			if filePath.is_file():
				for ohlc in RecordingPairFromPath.readOHLCFromFile(filePath):
					prices.addOHLCObject(ohlc, ignoreEqual=True)

			# Save the data to a file.
			with filePath.open(mode="w") as f:
				for ohlc in prices:
					content = f"{ohlc.timestamp};{ohlc.open};{ohlc.high};{ohlc.low};{ohlc.close};{ohlc.volume}"
					content += "".join([f";{e.kind.value};{e.value}" for e in ohlc.events])
					f.write(content + "\n")
