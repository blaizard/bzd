import pathlib
import typing

from apps.ebook.calibre.calibre import Calibre
from apps.ebook.flow import ActionInterface, FlowEnum
from apps.ebook.providers import ProviderEbook


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

		return [(ProviderEbook(ebook=output, metadata=provider.metadata), None)]
