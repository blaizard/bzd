import typing

from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entities.all import Use


class ProcessInclusions(VisitorBase[None]):
	"""
	Preprocess stage, look for all inclusion and pre-process them.
	"""

	def __init__(self, objectContext: typing.Any) -> None:
		super().__init__()
		self.objectContext = objectContext

	def visitUse(self, entity: Use, result: None) -> None:

		try:
			if not self.objectContext.isPreprocessed(source=entity.path.as_posix()):
				self.objectContext.preprocess(source=entity.path.as_posix())
			assert self.objectContext.isPreprocessed(source=entity.path.as_posix()), "Unable to find preprocessed file."

		except Exception as e:
			entity.error(message=str(e))
			raise
