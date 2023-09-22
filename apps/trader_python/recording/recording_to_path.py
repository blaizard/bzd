import pathlib
import json

from apps.trader_python.recording.recording import Price, Recording
from apps.trader_python.recording.recording_from_path import RecordingPairFromPath
from apps.trader_python.recording.recording_from_data import RecordingPairFromData


def _iterateByGroup(iterator, group) -> None:
	"""Iterate the recording by consecutive groups."""

	try:
		price = next(iterator)
	except StopIteration:
		return

	context = {"iterator": iterator, "price": price, "continue": True}

	while context["continue"]:

		def groupIterator(current, context) -> Price:
			while True:
				yield context["price"]
				try:
					context["price"] = next(context["iterator"])
				except StopIteration:
					context["continue"] = False
					break
				if current != group(context["price"]):
					break

		current = group(context["price"])
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

		# Save the prices
		for key, iterator in _iterateByGroup(pair.__iter__(), group=lambda x: x.date.strftime("%Y-%m")):
			prices = RecordingPairFromData.fromPrices(iterator, first="x", second="y")
			filePath = current / f"{key}.csv"

			# Merge entries with the existing one if any.
			if filePath.is_file():
				for price in RecordingPairFromPath.readPriceFromFile(filePath):
					prices.addPriceObject(price, ignoreEqual=True)

			# Save the data to a file.
			with filePath.open(mode="w") as f:
				for price in prices:
					content = f"{price.timestamp};{price.price};{price.volume}"
					content += "".join([f";{e.kind.value};{e.value}" for e in price.events])
					f.write(content + "\n")
