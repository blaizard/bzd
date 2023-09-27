import pathlib
import argparse
import pathlib
import sys
import os

from apps.trader_python.exchange.virtual.exchange import Virtual
from apps.trader_python.recording.recording_from_path import RecordingFromPath
from bzd.http.server import HttpServer

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Trader.")
	parser.add_argument("--frontend", type=pathlib.Path, default=None, help="The path of the frontend.")
	parser.add_argument("--port", type=int, default=0, help="The port for the HTTP server.")

	args = parser.parse_args()

	server = HttpServer(port=args.port)
	server.addStaticRoute("/", args.frontend)
	server.start()

	recording = RecordingFromPath(pathlib.Path("~/Documents/recordings/yfinance"))
	virtual = Virtual(recording)

	print(recording)
