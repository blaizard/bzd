import typing

from bdl.visitor import Visitor as VisitorBase
from bdl.entities.all import Preset, Use


class ProcessInclusions(VisitorBase[None]):
	"""
	Preprocess stage, look for all inclusion and pre-process them.
	"""

	def __init__(self, objectContext: typing.Any) -> None:
		super().__init__()
		self.objectContext = objectContext

	def visitUse(self, entity: Use, result: None) -> None:
		try:
			maybePreprocess = self.objectContext.findPreprocess(source=entity.path.as_posix())
			if maybePreprocess is None:
				self.objectContext.preprocess(source=entity.path.as_posix())
				assert self.objectContext.findPreprocess(source=entity.path.as_posix()), (
					f"Unable to find preprocessed file for '{entity.path}'."
				)

		except Exception as e:
			entity.error(message=str(e))
			raise

	def visitPreset(self, entity: Preset, result: None) -> None:
		"""Load preset JSON files at preprocess time."""
		maybeFile = self.objectContext.findFile(source=entity.path.as_posix())
		if maybeFile is None:
			entity.error(message=f"Cannot find preset file for '{entity.path}' (name '{entity.name}').")
			return
		try:
			content = maybeFile.read_text(encoding="utf-8")
		except (FileNotFoundError, OSError) as e:
			entity.error(message=f"Cannot read preset file '{maybeFile}' for '{entity.name}': {e}")
			return
		entity.setContent(content)
