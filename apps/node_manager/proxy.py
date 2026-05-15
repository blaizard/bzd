import argparse
import asyncio
import functools
import typing

from apps.node_manager.private.common import getHostPort


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
	clientReader: asyncio.StreamReader, clientWriter: asyncio.StreamWriter, targetHost: str, targetPort: int
) -> None:
	"""Handle incoming connection and routes it to the correct target port."""

	clientAddress = clientWriter.get_extra_info("peername")
	print(f"New connection from {clientAddress} -> {targetHost}:{targetPort}")

	try:
		targetReader, targetWriter = await asyncio.open_connection(targetHost, targetPort)
	except Exception as e:
		print(f"Failed to connect to target {targetHost}:{targetPort}: {e}")
		clientWriter.close()
		return

	# Pipe in both directions.
	pipToTarget = asyncio.create_task(pipe(clientReader, targetWriter))
	pipToClient = asyncio.create_task(pipe(targetReader, clientWriter))
	await asyncio.gather(pipToTarget, pipToClient)

	print(f"Connection closed for {clientAddress}")


async def main(bind: str, mapping: typing.Dict[int, str]) -> None:

	servers = []

	for listenPort, target in mapping.items():
		targetHost, targetPort = getHostPort(target)
		handler = functools.partial(handleCLient, targetHost=targetHost, targetPort=targetPort)
		server = await asyncio.start_server(handler, bind, listenPort)
		servers.append(server.serve_forever())
		print(f"Proxy started, listening at {bind}:{listenPort} -> {targetHost}:{targetPort}")

	await asyncio.gather(*servers)


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

	args = parser.parse_args()

	mapping = {int(port): target for port, target in args.proxy}

	asyncio.run(main(bind=args.bind, mapping=mapping))
