import os

from apps.node_manager.rest_server import RESTServerContext


def handlerSuspend(context: RESTServerContext) -> None:

	print("Suspending...")
	os.system("systemctl suspend")


handlersPower = {"/suspend": handlerSuspend}
