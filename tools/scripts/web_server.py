#!/usr/bin/python3

import argparse
import http.server
import os
import socket
import socketserver
import tarfile
import sys
import pathlib
import mimetypes
import functools
import typing


class WebServer(socketserver.TCPServer):
	allow_reuse_address = True

	def server_bind(self) -> None:
		"""Bind the server socket to an address."""

		super().server_bind()
		self.hostname, self.port = self.socket.getsockname()

	def run(self) -> None:
		try:
			self.serve_forever()
		except KeyboardInterrupt:
			print("\r<Keyboard interrupt>")
		finally:
			self.server_close()


class ArchiveRequestHandler(http.server.SimpleHTTPRequestHandler):

	def __init__(self, archive: pathlib.Path, *args: typing.Any, **kwargs: typing.Any) -> None:
		mimetypes.init()
		self.archive = tarfile.open(archive, "r")
		prefix = os.path.commonprefix(self.archive.getnames())
		prefix = "." if prefix == "." else "/".join(prefix.split("/")[:-1])
		self.prefix = prefix + "/" if prefix else ""
		super().__init__(*args, **kwargs)

	def do_GET(self) -> None:
		path = pathlib.Path("/index.html" if self.path == "/" else self.path)
		pathInArchive = self.prefix + str(path.relative_to('/'))

		try:
			reader = self.archive.extractfile(pathInArchive)
		except KeyError:
			self.send_response(http.HTTPStatus.NOT_FOUND)
			self.end_headers()
			return

		self.send_response(http.HTTPStatus.OK)
		self.send_header("Content-type", mimetypes.guess_type(path)[0] or "text/html")
		self.end_headers()
		if reader is not None:
			self.wfile.write(reader.read())


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Web server for testing purpose only.")
	parser.add_argument("-u",
		"--hostname",
		dest="hostname",
		default="0.0.0.0",
		type=str,
		help="The hostname to be used.")
	parser.add_argument("-p", "--port", dest="port", default=0, type=int, help="Port to be used.")
	parser.add_argument("-r",
		"--root",
		dest="root",
		default=None,
		type=str,
		help="Prefix path where the files to serve are located.")
	parser.add_argument("path",
		default=".",
		nargs='?',
		help="Serve files from this specific directory or archive, by default the current directory will be used.")

	args = parser.parse_args()

	handler: typing.Type[http.server.BaseHTTPRequestHandler]

	# If archive is set, serve files from this specific archive
	if os.path.isfile(args.path):
		print(f"Serving archive '{args.path}'.")
		handler = functools.partial(ArchiveRequestHandler, pathlib.Path(args.path))  # type: ignore

	# If special path, serve it
	elif os.path.isdir(args.path):
		print(f"Serving directory '{args.path}'.")
		handler = http.server.SimpleHTTPRequestHandler
		os.chdir(args.path)

	else:
		print(f"Invalid path '{args.path}'.")
		sys.exit(1)

	# If the root is somewhere else
	if args.root:
		os.chdir(args.root)

	server = WebServer((args.hostname, args.port), handler)
	print(f"Web server ready, serving '{args.path}' at 'http://{server.hostname}:{server.port}'.")
	server.run()
