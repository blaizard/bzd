import os

from bzd.http.server import RESTServerContext


def handlerSuspend(context: RESTServerContext) -> None:
	print("Suspending...")
	os.system("systemctl suspend")


def handlerShutdown(context: RESTServerContext) -> None:
	print("Shutting down...")
	os.system("systemctl poweroff")
