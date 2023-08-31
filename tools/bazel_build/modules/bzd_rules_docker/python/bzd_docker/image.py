import pathlib
import json


def image(identifier: str) -> str:
	"""Get the full name of the docker image.
    
    Args:
        identifier: The identifier of the image.
    """

	config = json.loads((pathlib.Path(__file__).parent.parent.parent / "config.json").read_text())
	assert (
	    identifier in config
	), f"The identifier '{identifier}' is not registered as a valid docker image, please check 'tools/docker_images/register.bzl'."

	item = config[identifier]
	return f"{item['image']}@{item['digest']}"
