import os
import typing

from bzd.http.server import RESTServerContext


def handlerSuspend(context: typing.Optional[RESTServerContext] = None) -> None:
	print("Suspending...")
	os.system("systemctl suspend")


def handlerShutdown(context: typing.Optional[RESTServerContext] = None) -> None:
	print("Shutting down...")
	os.system("systemctl poweroff")
