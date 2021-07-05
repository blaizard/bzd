from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entities.all import Use


class Preprocess(VisitorBase[None]):
	"""
	Preprocess stage, look for all inclusion and make pre-process them.
	"""

	def __init__(self, objectContext) -> None:
		super().__init__()
		self.objectContext = objectContext

	def visitUse(self, entity: Use, result: None) -> None:

		try:
			if not self.objectContext.isPreprocessed(path=entity.path):
				self.objectContext.preprocess(path=entity.path)
			assert self.objectContext.isPreprocessed(path=entity.path), "Unable to find preprocessed file."

		except Exception as e:
			entity.error(message=str(e))
			raise
