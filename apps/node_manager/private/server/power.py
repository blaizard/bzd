import os

from bzd.http.server import RESTServerContext


def handlerSuspend(context: RESTServerContext | None = None) -> None:
	print("Suspending...")
	os.system("systemctl suspend")


def handlerShutdown(context: RESTServerContext | None = None) -> None:
	print("Shutting down...")
	os.system("systemctl poweroff")
