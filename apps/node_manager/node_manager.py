import argparse
import json
import sys
import os
import pathlib

from apps.node_manager.private.server.power import handlerSuspend, handlerShutdown
from apps.node_manager.private.server.monitor import Monitor
from apps.node_manager.private.server.config import Config
from apps.node_manager.private.server.workload import Workload
from bzd.utils.scheduler import Scheduler
from bzd.sync.singleton import Singleton
from apps.artifacts.api.python.node.node import Node
from bzd.http.server import HttpServer

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Node Manager.")
	parser.add_argument("--bind", default="0.0.0.0", help="Address to bind.")
	parser.add_argument("--port", default=9999, type=int, help="Port to bind.")
	parser.add_argument(
		"--report-rate",
		default=5,
		type=int,
		help="The rate (in s) at which status messages are emitted.",
	)
	parser.add_argument(
		"--power",
		action="store_true",
		help="Whether or not the power on this node can be controlled.",
	)
	parser.add_argument(
		"--node-token",
		type=str,
		default=os.environ.get("BZD_NODE_TOKEN"),
		help="A token to be used to access the node server.",
	)
	parser.add_argument("--config", type=pathlib.Path, help="Configuration for the manager.")
	parser.add_argument(
		"--default-termination-period", type=float, default=3 * 60 * 60, help="The default termination period in seconds."
	)
	parser.add_argument(
		"uid",
		nargs="?",
		default=os.environ.get("BZD_NODE_UID"),
		help="The UID of this node. If no UID is provided, the application will report the monitoring on the command line.",
	)

	args = parser.parse_args()

	def terminateFn() -> None:
		print("Terminate (no workload)")

	# Instantiate the monitor.
	config = Config(path=args.config)
	workload = Workload(terminateFn=terminateFn, defaultTerminationPeriodS=args.default_termination_period)
	monitor = Monitor(config=config, workload=workload if args.power else None)

	if args.uid is None:
		data = monitor.all()
		print(json.dumps(data, indent=4))
		sys.exit(0)

	# Ensure only a single instance of this program is running at a time.
	singleton = Singleton.makeFromName("node_manager")
	if not singleton.lock():
		print("Another instance of 'node_manager' is already running, aborting.")
		sys.exit(0)

	node = Node(uid=args.uid, token=args.node_token)

	# Start the thread to monitor the node.
	def monitorWorkload() -> None:
		data = monitor.all()
		try:
			node.publish(data=data)
		except Exception:
			# Ignore any errors, we don't want to crash if something is wrong on the server side.
			pass

	scheduler = Scheduler()
	server = HttpServer(port=args.port, bind=args.bind)

	scheduler.add("monitor", args.report_rate, monitorWorkload)
	handlers = {
		"get": {
			"/monitor": monitor.handlerMonitor,
		},
		"post": {},
	}

	if args.power:
		handlers["get"].update(
			{
				"/suspend": handlerSuspend,
				"/shutdown": handlerShutdown,
			}
		)
		handlers["post"].update(
			{
				"/workload/register": workload.handlerRegister,
				"/workload/heartbeat": workload.handlerHeartBeat,
				"/workload/release": workload.handlerRelease,
			}
		)
		scheduler.add("termination_watcher", 60, workload.terminationWatcher)

	for method, data in handlers.items():
		for path, handler in data.items():
			server.addRoute(method=method, uri=path, handler=handler)

	try:
		scheduler.start()
		server.start()
	finally:
		scheduler.stop()
