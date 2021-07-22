from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use


class Validation(VisitorBase[None]):

	def visitExpression(self, entity: Expression, result: None) -> None:

		entity.contracts.validate()

		entity.assertTrue(condition=self.parent is not None, message="Expression must be in a nested context.")
		if not entity.isName:
			assert self.parent
			entity.assertTrue(condition=self.parent.category == "composition",
				message="Unamed expressions can only be in a composition context, not a '{}' context.".format(
				self.parent.category))

	def visitNestedEntities(self, entity: Nested, result: None) -> None:

		entity.contracts.validate()

		if entity.type == "interface":
			entity.assertTrue(condition=not entity.hasInheritance, message="Interfaces cannot have inheritance.")
			entity.assertTrue(condition=entity.isName, message="Interfaces must have a valid name.")

		elif entity.type == "struct":
			entity.assertTrue(condition=entity.isName, message="Structures must have a valid name.")

		elif entity.type == "component":
			entity.assertTrue(condition=entity.isName, message="Components must have a valid name.")

		elif entity.type == "composition":
			entity.assertTrue(condition=not entity.hasInheritance, message="Compositions cannot have inheritance.")

		else:
			entity.assertTrue(condition=False, message="Unsupported nested type: '{}'.".format(entity.type))

	def visitMethod(self, entity: Method, result: None) -> None:

		entity.contracts.validate()

		# Arguments
		argumentNames = set()
		for arg in entity.args:
			arg.assertTrue(condition=arg.isName, message="Argument is missing a name.")
			arg.assertTrue(condition=(arg.name not in argumentNames),
				message="Argument name '{}' is already defined.".format(arg.name))
			argumentNames.add(arg.name)
			arg.contracts.validate()

	def visitUsing(self, entity: Using, result: None) -> None:

		entity.contracts.validate()

	def visitEnum(self, entity: Enum, result: None) -> None:

		entity.contracts.validate()
