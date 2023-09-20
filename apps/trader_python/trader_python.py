import pathlib

from apps.trader_python.exchange.virtual.exchange import Virtual
from apps.trader_python.recording.recording_from_path import RecordingFromPath

if __name__ == "__main__":

	recording = RecordingFromPath(pathlib.Path("~/Documents/recordings/yfinance"))
	virtual = Virtual(recording)

	print(recording)
