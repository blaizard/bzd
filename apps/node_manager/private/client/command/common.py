import typing


def getHostPort(hostPort: str) -> typing.Tuple[str, int]:
	"""Separate the hostname from the port and return it."""

	split = hostPort.split(":")
	assert len(split) == 2, f"The string does not have a correct format, should be 'hostname:port', not '{hostPort}'."
	return split[0], int(split[1])
