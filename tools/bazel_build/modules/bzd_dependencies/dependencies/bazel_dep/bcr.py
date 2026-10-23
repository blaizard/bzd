import typing

from bzd.http.client import HttpClient, HttpClientException, HttpResponse

from bzd_dependencies.utils.version import Version

DEFAULT_BCR_URL = "https://bcr.bazel.build"


class BcrRegistry:
	"""Read module metadata from the Bazel Central Registry."""

	def __init__(
		self,
		get: typing.Callable[[str], HttpResponse] = HttpClient.get,
		baseUrl: str = DEFAULT_BCR_URL,
	) -> None:
		self.get_ = get
		self.baseUrl_ = baseUrl.rstrip("/")
		self.versions_: typing.Dict[str, typing.Optional[typing.List[str]]] = {}

	def getVersions(self, module: str) -> typing.Optional[typing.List[str]]:
		"""Get the versions published for a module, or None if the module is not in the BCR."""

		if module not in self.versions_:
			try:
				response = self.get_(f"{self.baseUrl_}/modules/{module}/metadata.json")
			except HttpClientException as exception:
				if exception.status != 404:
					raise
				self.versions_[module] = None
			else:
				self.versions_[module] = [str(entry) for entry in response.json.get("versions", [])]
		return self.versions_[module]

	def getLatestStable(self, module: str, currentVersion: str) -> typing.Optional[str]:
		"""Get the latest stable version of a module strictly newer than the current one, if any."""

		current = Version.parse(currentVersion)
		if current is None:
			return None
		latest: typing.Optional[Version] = None
		for candidate in self.getVersions(module) or []:
			parsed = Version.parse(candidate)
			if parsed is None or not parsed.isStable or parsed <= current:
				continue
			if latest is None or parsed > latest:
				latest = parsed
		return str(latest) if latest is not None else None
