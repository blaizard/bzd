"""Manage the Docker registry."""

import typing

from bzd.http.client import HttpClient


class DockerRegistry:

	def __init__(self, url: str) -> None:
		self.url = url

	def listRepositories(self) -> typing.List[str]:
		"""List all repositories of this registry."""

		response = HttpClient.get(f"{self.url}/v2/_catalog")
		return response.json.get("repositories") or []

	def listTags(self, repository: str) -> typing.List[str]:
		"""List all tags associated with a repository."""

		response = HttpClient.get(f"{self.url}/v2/{repository}/tags/list")
		return response.json.get("tags") or []  # "tags" can be None

	def getDigest(self, repository: str, tag: str) -> str:
		"""Get the digest from a tag."""

		response = HttpClient.get(f"{self.url}/v2/{repository}/manifests/{tag}",
		                          headers={
		                              "Accept": "application/vnd.docker.distribution.manifest.v2+json,"
		                                        "application/vnd.docker.distribution.manifest.list.v2+json,"
		                                        "application/vnd.oci.image.manifest.v1+json,"
		                                        "application/vnd.oci.image.index.v1+json"
		                          })
		return response.getHeader("Docker-Content-Digest")

	def getImages(self) -> typing.Dict[str, typing.Dict[str, str]]:
		"""Get a dictionary of images, their tags and digests."""

		images: typing.Dict[str, typing.Dict[str, str]] = {}
		repositories = self.listRepositories()
		for repository in repositories:
			images[repository] = {}
			tags = self.listTags(repository)
			for tag in tags:
				digest = self.getDigest(repository, tag)
				images[repository][tag] = digest
		return images

	def delete(self, repository: str, digest: str) -> None:
		"""Delete an image."""

		HttpClient.delete(f"{self.url}/v2/{repository}/manifests/{digest}",
		                  headers={
		                      "Accept": "application/vnd.docker.distribution.manifest.v2+json,"
		                                "application/vnd.docker.distribution.manifest.list.v2+json,"
		                                "application/vnd.oci.image.manifest.v1+json,"
		                                "application/vnd.oci.image.index.v1+json"
		                  })
