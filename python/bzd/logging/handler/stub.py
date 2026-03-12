from bzd.logging.handler import LoggerHandler, LoggerHandlerData, LoggerHandlerFlow


class LoggerHandlerStub(LoggerHandler):
    """Stub that outputs nothing."""

    def handler(self, data: LoggerHandlerData, flow: LoggerHandlerFlow) -> None:
        return
