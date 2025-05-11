import pathlib
import typing

from tools.docker_images.ebook.calibre.calibre import Calibre
from tools.docker_images.ebook.flow import ActionInterface, FlowEnum
from tools.docker_images.ebook.providers import ProviderEbook


class RemoveDRM(ActionInterface):
	"""Remove the DRM of an ebook."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderEbook

	def __init__(self, calibreConfigPath: pathlib.Path) -> None:
		super().__init__()
		self.calibre = Calibre(configPath=calibreConfigPath)

	def process(self, provider: ProviderEbook,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderEbook, typing.Optional[FlowEnum]]]:
		if not provider.hasKeys:
			print("No keys provided, therefore not removing the DRM.")
			return [(provider, None)]

		for key in provider.keys:
			self.calibre.addKey(key)

		output = directory / provider.ebook.name
		self.calibre.sanitize(provider.ebook, output)

		return [(ProviderEbook(ebook=output), None)]
