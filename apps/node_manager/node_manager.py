import argparse
import json
import threading
import sys

from apps.node_manager.rest_server import RESTServer, RESTServerContext
from apps.node_manager.power import handlersPower
from apps.node_manager.monitor import handlersMonitor, monitor
from apps.node_manager.singleton import instanceAlreadyRunning
from bzd.http.client import HttpClient
from bzd.utils.scheduler import Scheduler
from apps.artifacts.api.python.node.node import Node

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="WOL manager.")
	parser.add_argument("--bind", default="0.0.0.0", help="Address to bind.")
	parser.add_argument("--port", default=9999, type=int, help="Port to bind.")
	parser.add_argument("--report-rate",
	                    default=5,
	                    type=int,
	                    help="The rate (in s) at which status messages are emitted.")
	parser.add_argument("--power", action="store_true", help="Whether or not the power on this node can be controlled.")
	parser.add_argument(
	    "uid",
	    nargs="?",
	    default=None,
	    help=
	    "The UID of this node. If no UID is provided, the application will report the monitoring on the command line.")

	args = parser.parse_args()

	if args.uid is None:
		data = monitor()
		print(json.dumps(data, indent=4))
		sys.exit(0)

	# Ensure only a single instance of this program is running at a time.
	if instanceAlreadyRunning("node_manager"):
		print("Another instance of 'node_manager' is already running, aborting.")
		sys.exit(0)

	node = Node()

	# Start the thread to monitor the node.
	def monitorWorkload() -> None:
		data = monitor()
		try:
			node.publish(uid=args.uid, data=data)
		except:
			# Ignore any errors, we don't want to crash if something is wrong on the server side.
			pass

	scheduler = Scheduler()
	scheduler.add("monitor", args.report_rate, monitorWorkload)
	scheduler.start()

	try:
		handlers = {**handlersMonitor}
		if args.power:
			handlers.update(**handlersPower)

		server = RESTServer(args.bind, args.port, handlers)
		print(f"Server started at http://{args.bind}:{args.port}", flush=True)
		server.run()

	finally:
		scheduler.stop()
