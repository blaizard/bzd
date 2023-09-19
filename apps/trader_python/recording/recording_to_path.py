import pathlib

from apps.trader_python.recording.recording import Recording
from apps.trader_python.recording.recording_from_path import RecordingPairFromPath
from apps.trader_python.recording.recording_from_data import RecordingPairFromData


def recordingToPath(recording: Recording, path: pathlib.Path) -> None:
	"""Save a recording to a specific path."""

	resolvedPath = path.expanduser().resolve()

	for pair in recording:
		current = resolvedPath / pair.uid.replace("/", "-")
		current.mkdir(parents=True, exist_ok=True)

		# Save the prices
		for key, iterator in pair.iterateByGroup(group=lambda x: x.date.strftime("%Y-%m")):
			prices = RecordingPairFromData.fromPrices(iterator)
			filePath = current / f"{key}.csv"

			# Merge entries with the existing one if any.
			if filePath.is_file():
				for price in RecordingPairFromPath.readPriceFromFile(filePath):
					prices.addPriceObject(price, ignoreEqual=True)

			# Save the data to a file.
			with filePath.open(mode="w") as f:
				for price in prices:
					f.write(f"{price.timestamp};{price.price};{price.volume}\n")
