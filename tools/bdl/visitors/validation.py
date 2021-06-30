from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use


class Validation(VisitorBase[None]):

	def visitExpression(self, entity: Expression, result: None) -> None:
		entity.assertTrue(condition=self.parent is not None, message="Expression must be in a nested context.")
		if not entity.isName:
			assert self.parent
			entity.assertTrue(condition=self.parent.category == "composition",
				message="Unamed expressions can only be in a composition context.")
