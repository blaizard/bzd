#!/usr/bin/python3

import argparse
import http.server
import os
import socketserver
import tarfile
import tempfile


class WebServer(socketserver.TCPServer):
	allow_reuse_address = True

	def run(self) -> None:
		try:
			self.serve_forever()
		except KeyboardInterrupt:
			print("\r<Keyboard interrupt>")
		finally:
			self.server_close()


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Web server for testing purpose only.")
	parser.add_argument("-p", "--port", dest="port", default=8080, type=int, help="Port to be used.")
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
				print("Extracting content of '{}' to temporary directory '{}'.".format(args.path, tempPath.name))
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
		server = WebServer(("", args.port), handler)
		print("Web server ready, serving '{}' at 'http://localhost:{}'.".format(args.path, args.port))
		server.run()

	# Cleanup RAII style
	finally:
		if tempPath:
			print("Cleaning up temporary directory '{}'.".format(tempPath.name))
			tempPath.cleanup()
