import argparse
import asyncio
import functools
import typing
from contextlib import contextmanager

from bzd.logging import Logger
from bzd.logging.handler.stderr import LoggerHandlerStderr
from apps.node_manager.private.common import getHostPort
from apps.node_manager.client import ARGUMENTS, commandWoLLeasePeriod
from apps.node_manager.private.client.command.lease import leaseContext


class WolLeaseManager:
	def __init__(
		self,
		mac: str,
		broadcast: str,
		service: str,
		wait: typing.List[str],
		timeout: int,
		server: str,
		name: str,
		ttl: int,
		gracePeriodS: int,
		logger: Logger,
	) -> None:
		self.mac = mac
		self.broadcast = broadcast
		self.service = service
		self.wait = wait
		self.timeout = timeout
		self.server = server
		self.name = name
		self.ttl = ttl
		self.gracePeriodS = gracePeriodS
		self.logger = logger
		self.activeClients = 0
		self.clientEvent = asyncio.Event()

	async def process(self) -> None:

		while True:
			# Wait for a client to be active.
			if self.activeClients == 0:
				self.clientEvent.clear()
				await self.clientEvent.wait()
				continue

			self.logger.info(f"{self.activeClients} client(s), leasing...")

			try:
				leaseId = commandWoLLeasePeriod(
					mac=self.mac,
					broadcast=self.broadcast,
					service=self.service,
					wait=self.wait,
					timeout=self.timeout,
					server=self.server,
					name=self.name,
					logger=self.logger,
					# WoL and lease for 60s.
					ttl=60,
				)
				with leaseContext(name=self.name, server=self.server, ttl=self.ttl, leaseId=leaseId, logger=self.logger) as stop:
					while not stop.is_set():
						await asyncio.sleep(1)
						if self.activeClients == 0:
							self.clientEvent.clear()
							try:
								await asyncio.wait_for(self.clientEvent.wait(), timeout=self.gracePeriodS)
							except TimeoutError:
								if self.activeClients == 0:
									break

			except Exception as e:
				self.logger.error(f"Lease error: {e}")
				await asyncio.sleep(5)

			self.logger.info("Leasing completed.")

	@contextmanager
	def lease(self) -> typing.Generator[None, None, None]:
		self.activeClients += 1
		self.clientEvent.set()
		try:
			yield
		finally:
			self.activeClients -= 1
			self.clientEvent.set()


async def pipe(reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
	"""Continuously reads from one stream and writes to another."""

	try:
		while True:
			data = await reader.read(4096)
			if not data:
				break
			writer.write(data)
			await writer.drain()
	except Exception:
		pass
	finally:
		writer.close()


async def handleCLient(
	clientReader: asyncio.StreamReader,
	clientWriter: asyncio.StreamWriter,
	targetHost: str,
	targetPort: int,
	wolLeaseManager: WolLeaseManager,
) -> None:
	"""Handle incoming connection and routes it to the correct target port."""

	with wolLeaseManager.lease():
		try:
			targetReader, targetWriter = await asyncio.open_connection(targetHost, targetPort)
		except Exception:
			clientWriter.close()
			return

		# Pipe in both directions.
		pipToTarget = asyncio.create_task(pipe(clientReader, targetWriter))
		pipToClient = asyncio.create_task(pipe(targetReader, clientWriter))
		await asyncio.gather(pipToTarget, pipToClient)


async def main(bind: str, mapping: typing.Dict[int, str], wolLeaseManager: WolLeaseManager, logger: Logger) -> None:

	servers = []

	for listenPort, target in mapping.items():
		targetHost, targetPort = getHostPort(target)
		handler = functools.partial(
			handleCLient, targetHost=targetHost, targetPort=targetPort, wolLeaseManager=wolLeaseManager
		)
		server = await asyncio.start_server(handler, bind, listenPort)
		servers.append(server.serve_forever())
		logger.info(f"Proxy started, listening at {bind}:{listenPort} -> {targetHost}:{targetPort}")

	await asyncio.gather(wolLeaseManager.process(), *servers)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Node manager proxy client.")
	parser.add_argument(
		"--bind",
		type=str,
		default="0.0.0.0",
		help="Default address to bind to.",
	)
	parser.add_argument(
		"--proxy",
		nargs=2,
		action="append",
		default=[],
		metavar=("<BIND PORT>", "<TARGET HOST:PORT>"),
		help="Port to target proxy mapping.",
	)
	for arg in ["broadcast", "service", "timeout", "wait", "mac", "server", "name", "ttl"]:
		parser.add_argument(
			f"--{arg}",
			**ARGUMENTS[arg],
		)

	args = parser.parse_args()

	logger = Logger("node_manager").handlers(LoggerHandlerStderr())

	mapping = {int(port): target for port, target in args.proxy}

	wolLeaseManager = WolLeaseManager(
		mac=args.mac,
		broadcast=args.broadcast,
		service=args.service,
		wait=args.wait,
		timeout=args.timeout,
		server=args.server,
		name=args.name,
		ttl=args.ttl,
		gracePeriodS=5,
		logger=logger,
	)
	asyncio.run(main(bind=args.bind, mapping=mapping, wolLeaseManager=wolLeaseManager, logger=logger))
