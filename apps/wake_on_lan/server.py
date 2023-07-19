import argparse
import http.server
import socketserver
from http import HTTPStatus
import os


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
		if self.path == '/suspend':

			# Reply.
			self.send_response(HTTPStatus.OK)
			self.end_headers()

			# Execute the suspend command.
			print("Suspending...")
			os.system("systemctl suspend")

		else:
			self.send_response(HTTPStatus.NOT_FOUND)
			self.end_headers()


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="WOL manager.")
	parser.add_argument("-b", "--bind", default="0.0.0.0", help="Address to bind.")
	parser.add_argument("-p", "--port", default=8000, type=int, help="Port to bind.")

	args = parser.parse_args()

	server = HTTPServer((args.bind, args.port), Handler)
	print(f"Server started at {args.bind}:{args.port}")
	server.serve_forever()
