import pathlib
import json


def image(identifier: str) -> str:
	"""Get the full name of the docker image."""

	config = json.loads(pathlib.Path("tools/docker_images/config.json").read_text())
	assert (
	    identifier in config
	), f"The identifier '{identifier}' is not registered as a valid docker image, please check 'tools/docker_images/register.bzl'."

	item = config[identifier]
	return f"{item['registry']}/{item['repository']}@{item['digest']}"
