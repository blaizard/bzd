"""Read docker compose files."""

import typing


class DockerCompose:

	def __init__(self, data: typing.Dict[str, typing.Any]) -> None:
		self.data = data

	def getImages(self) -> typing.Dict[str, typing.Set[str]]:
		"""Extract all images and their tags from the docker compose file."""

		images = {}
		for service in self.data.get("services", {}).values():
			if "image" in service:
				image = service["image"]
				imageSplit = image.split("/")
				isRegistry = len(imageSplit) > 1 and ("." in imageSplit[0] or ":" in imageSplit[0])
				# Remove the registry part if any.
				if isRegistry:
					imageSplit.pop(0)
				image = "/".join(imageSplit)
				# Extract tag from image.
				if ":" in image:
					repository, tag = image.rsplit(":", maxsplit=1)
				else:
					repository = image
					tag = "latest"
				images.setdefault(repository, set())
				images[repository].add(tag)
		return images
