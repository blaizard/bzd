import argparse
import json
import threading
import sys

from apps.node_manager.rest_server import RESTServer, RESTServerContext
from apps.node_manager.power import handlersPower
from apps.node_manager.monitor import handlersMonitor, monitor
from apps.node_manager.singleton import instanceAlreadyRunning
from bzd.http.client import HttpClient


class TimerThread(threading.Thread):

	def __init__(self, workload, periodS: float) -> None:
		super().__init__(daemon=True)
		self.periodS = periodS
		self.event = threading.Event()
		self.workload = workload

	def run(self) -> None:
		while not self.event.wait(self.periodS):
			self.workload()

	def stop(self) -> None:
		self.event.set()
		self.join()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="WOL manager.")
	parser.add_argument("--bind", default="0.0.0.0", help="Address to bind.")
	parser.add_argument("--port", default=9999, type=int, help="Port to bind.")
	parser.add_argument("--report-endpoint",
	                    default="http://data.master.blaizard.com/x/nodes",
	                    type=str,
	                    help="The endpoint to report data.")
	parser.add_argument("--report-rate",
	                    default=5,
	                    type=int,
	                    help="The rate (in s) at which status messages are emitted.")
	parser.add_argument("--power", action="store_true", help="Wether or not the power on this node can be controlled.")
	parser.add_argument("uid", help="The UID of this node.")

	args = parser.parse_args()

	# Ensure only a single instance of this program is running at a time.
	if instanceAlreadyRunning("node_manager"):
		sys.exit(0)

	# Start the thread to monitor the node.
	def monitorWorkload() -> None:
		data = monitor()
		try:
			HttpClient.post(f"{args.report_endpoint}/{args.uid}/data", json=data)
		except:
			# Ignore any errors, we don't want to crash if something is wrong on the server side.
			pass

	monitorThread = TimerThread(monitorWorkload, args.report_rate)
	monitorThread.start()

	try:
		handlers = {**handlersMonitor}
		if args.power:
			handlers.update(**handlersPower)

		server = RESTServer(args.bind, args.port, handlers)
		print(f"Server started at http://{args.bind}:{args.port}")
		server.run()

	finally:
		monitorThread.stop()
