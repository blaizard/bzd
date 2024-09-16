import http.server
import socketserver
import typing
import pathlib
import mimetypes
from http import HTTPStatus
from functools import partial

Handlers = typing.Mapping[str, typing.Mapping[str, typing.Callable[["RESTServerContext"], None]]]
StaticRoutes = typing.Mapping[str, pathlib.Path]

METHODS_ = {"get", "post", "put", "delete", "head", "patch"}


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

	def __init__(self, handlers: Handlers, staticRoutes: StaticRoutes, *args: typing.Any, **kwargs: typing.Any) -> None:
		self.handlers = handlers
		self.staticRoutes = staticRoutes
		super().__init__(*args, **kwargs)

	def _handleStatic(self) -> bool:

		for uri, path in self.staticRoutes.items():
			if self.path.lower().startswith(uri):

				requestedPath = self.path[len(uri):].strip("/")
				requestedPath = requestedPath if requestedPath else "index.html"
				fullPath = (path / requestedPath)
				if not fullPath.is_file():
					return False

				self.send_response(http.HTTPStatus.OK)
				self.send_header("Content-type", mimetypes.guess_type(fullPath)[0] or "text/html")
				self.end_headers()
				with fullPath.open(mode="rb") as f:
					self.wfile.write(f.read())

				return True
		return False

	def _notFound(self) -> None:
		self.send_response(HTTPStatus.NOT_FOUND)
		self.end_headers()

	def _handle(self, method: str) -> bool:
		if self.path in self.handlers[method]:
			context = RESTServerContext()

			try:
				self.handlers[method][self.path](context)
			except Exception as e:
				self.send_response(HTTPStatus.INTERNAL_SERVER_ERROR)
				self.end_headers()
				self.wfile.write(str(e))
				return True

			# Write the response.
			self.send_response(HTTPStatus.OK)
			for header in context.headers:
				self.send_header(header[0], header[1])
			self.end_headers()
			for data in context.data:
				self.wfile.write(data)
			return True

		return False

	def _handleREST(self, method: str) -> None:
		if self._handle(method):
			pass
		else:
			self._notFound()

	def do_GET(self) -> None:
		if self._handle("get"):
			pass
		elif self._handleStatic():
			pass
		else:
			self._notFound()

	def do_POST(self) -> None:
		self._handleREST("post")

	def do_PUT(self) -> None:
		self._handleREST("put")

	def do_DELETE(self) -> None:
		self._handleREST("delete")

	def do_HEAD(self) -> None:
		self._handleREST("head")

	def do_PATCH(self) -> None:
		self._handleREST("patch")


class HttpServer(socketserver.TCPServer):
	allow_reuse_address = True

	def __init__(self, port: int = 0, bind: str = "0.0.0.0") -> None:
		self.port = port
		self.bind = bind
		self.handlers = {method: {} for method in METHODS_}
		self.staticRoutes: StaticRoutes = {}
		super().__init__((bind, port), partial(Handler, self.handlers, self.staticRoutes))

	def server_bind(self) -> None:
		"""Bind the server socket to an address."""

		super().server_bind()
		self.bind, self.port = self.socket.getsockname()

	def addRoute(self, method: str, uri: str, handler: Handlers) -> None:
		method = method.lower()
		assert method in self.handlers, f"The route method {method} is not valid, expected any of {str(self.handlers.keys())}."
		assert uri not in self.handlers[method], f"The route '{uri}' is already set for '{method}'."

		self.handlers[method][uri] = handler

	def addStaticRoute(self, uri: str, path: pathlib.Path) -> None:
		assert uri not in self.staticRoutes, f"The static route '{uri}' is already set."

		self.staticRoutes[uri] = path

	def start(self) -> None:
		try:
			print(f"Serving http://{self.bind}:{self.port}")
			self.serve_forever()
		except KeyboardInterrupt:
			print("\r<Keyboard interrupt>")
		finally:
			self.server_close()
