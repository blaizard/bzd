import argparse
import json

from apps.node_manager.rest_server import RESTServer, RESTServerContext
from apps.node_manager.power import handlersPower
from apps.node_manager.monitor import handlersMonitor, monitor

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="WOL manager.")
	parser.add_argument("-b", "--bind", default="0.0.0.0", help="Address to bind.")
	parser.add_argument("-p", "--port", default=8000, type=int, help="Port to bind.")
	parser.add_argument("--server", action="store_true", help="Start the server")

	args = parser.parse_args()

	if args.server:
		handlers = {**handlersPower, **handlersMonitor}

		server = RESTServer(args.bind, args.port, handlers)
		print(f"Server started at http://{args.bind}:{args.port}")
		server.run()

	else:
		print(json.dumps(monitor(), indent=4))
