import os

from apps.node_manager.rest_server import RESTServerContext


def handlerSuspend(context: RESTServerContext) -> None:
	print("Suspending...")
	os.system("systemctl suspend")


def handlerShutdown(context: RESTServerContext) -> None:
	print("Shutting down...")
	os.system("systemctl poweroff")


handlersPower = {
    "/suspend": handlerSuspend,
    "/shutdown": handlerShutdown,
}
