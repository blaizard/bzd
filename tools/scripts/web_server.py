#!/usr/bin/python3

import argparse
import http.server
import os
import socket
import socketserver
import tarfile
import tempfile


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


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Web server for testing purpose only.")
	parser.add_argument("-h",
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

	tempPath = None
	try:

		# If archive is set, serve files from this specific archive
		if os.path.isfile(args.path):
			tempPath = tempfile.TemporaryDirectory()
			package = tarfile.open(args.path)
			try:
				print(f"Extracting content of '{args.path}' to temporary directory '{tempPath.name}'.")
				package.extractall(tempPath.name)
			finally:
				package.close()
			os.chdir(tempPath.name)

		# If special path, serve it
		elif os.path.isdir(args.path):
			os.chdir(args.path)

		# If the root is somewhere else
		if args.root:
			os.chdir(args.root)

		handler = http.server.SimpleHTTPRequestHandler
		server = WebServer((args.hostname, args.port), handler)
		print(f"Web server ready, serving '{args.path}' at 'http://{server.hostname}:{server.port}'.")
		server.run()

	# Cleanup RAII style
	finally:
		if tempPath:
			print(f"Cleaning up temporary directory '{tempPath.name}'.")
			tempPath.cleanup()
