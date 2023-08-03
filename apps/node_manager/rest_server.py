import http.server
import socketserver
import typing
from http import HTTPStatus
from functools import partial

Handlers = typing.Mapping[str, typing.Callable[["RESTServerContext"], None]]


class RESTServerContext:

	def __init__(self) -> None:
		self.headers: typing.List[typing.Tuple[str, str]] = []
		self.data: typing.List[typing.Any] = []

	def header(self, key: str, value: str) -> None:
		self.headers.append((key, value))

	def write(self, data: typing.Any) -> None:
		if isinstance(data, str):
			data = data.encode()
		self.data.append(data)


class Handler(http.server.SimpleHTTPRequestHandler):

	def __init__(self, handlers: Handlers, *args: typing.Any, **kwargs: typing.Any) -> None:
		self.handlers = handlers
		super().__init__(*args, **kwargs)

	def do_GET(self) -> None:
		if self.path in self.handlers:
			context = RESTServerContext()
			self.handlers[self.path](context)

			self.send_response(HTTPStatus.OK)
			for header in context.headers:
				self.send_header(header[0], header[1])
			self.end_headers()
			for data in context.data:
				self.wfile.write(data)

		else:
			self.send_response(HTTPStatus.NOT_FOUND)
			self.end_headers()


class RESTServer(socketserver.TCPServer):
	allow_reuse_address = True

	def __init__(self, bind: str, port: int, handlers: Handlers) -> None:
		super().__init__((bind, port), partial(Handler, handlers))

	def run(self) -> None:
		try:
			self.serve_forever()
		except KeyboardInterrupt:
			print("\r<Keyboard interrupt>")
		finally:
			self.server_close()
