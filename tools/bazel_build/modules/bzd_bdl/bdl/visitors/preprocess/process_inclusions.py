import typing

from bdl.visitor import Visitor as VisitorBase
from bdl.entities.all import Use


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
				assert self.objectContext.findPreprocess(
				    source=entity.path.as_posix()), f"Unable to find preprocessed file for '{entity.path}'."

		except Exception as e:
			entity.error(message=str(e))
			raise
