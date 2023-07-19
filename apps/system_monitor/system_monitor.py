import traceback
import json
import typing
import argparse
import http.server
import socketserver
from http import HTTPStatus

from bzd.utils.dict import updateDeep
from apps.system_monitor.modules.xsensors import XSensors
from apps.system_monitor.modules.py_psutil import PyPsUtil

supported = [
    #XSensors,
    PyPsUtil
]


def generateData() -> typing.Any:
	data: typing.Any = {}

	for SupportedClass in supported:
		instance = SupportedClass()
		try:
			if instance.available():
				updateDeep(data, instance.get())
		except Exception as e:
			print(f"Error with module '{str(SupportedClass.__name__)}': {str(e)}")
			traceback.print_exc()

	return data


def getHostPort(hostport: str) -> typing.Tuple[str, int]:
	"""Separate the hostname from the port and return it."""

	split = hostport.split(":")
	assert len(split) == 2, f"The string does not have a correct format, should be 'hostname:port', not '{hostport}'."
	return split[0], int(split[1])


class HTTPServer(socketserver.TCPServer):
	allow_reuse_address = True

	def run(self) -> None:
		try:
			self.serve_forever()
		except KeyboardInterrupt:
			print("\r<Keyboard interrupt>")
		finally:
			self.server_close()


class Handler(http.server.SimpleHTTPRequestHandler):

	def do_GET(self) -> None:
		if self.path == '/data':

			data = generateData()

			self.send_response(HTTPStatus.OK)
			self.send_header('Content-type', 'application/json')
			self.end_headers()
			self.wfile.write(json.dumps(data).encode())

		else:
			self.send_response(HTTPStatus.NOT_FOUND)
			self.end_headers()


if __name__ == "__main__":
	"""Data format output:
	temperatures: {
		"cpu": [60, 56],
		"gpu": [67, 63, 57, 59]
	}
	"""

	parser = argparse.ArgumentParser(description="System monitor.")
	parser.add_argument("-s", "--server", default=False, help="Server address:port to bind.")
	args = parser.parse_args()

	if args.server:

		host, port = getHostPort(args.server)
		server = HTTPServer((host, port), Handler)
		print(f"Server started at {host}:{port}")
		server.serve_forever()

	else:
		data = generateData()
		print(json.dumps(data, indent=4))
