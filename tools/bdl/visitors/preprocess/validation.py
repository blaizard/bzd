from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use, EntityType


class Validation(VisitorBase[None]):

	def validateContract(self, entity: EntityType) -> None:

		entity.contracts.validate()

		if entity.contracts.get("template"):
			if self.parent:
				entity.assertTrue(condition=self.parent.category == "config",
					message="Contract template can only be used in a config context, not a '{}' context.".format(
					self.parent.category))
			else:
				entity.error(message="Contract template can only be used in a config context, not at top level.")

	def visitExpression(self, entity: Expression, result: None) -> None:

		self.validateContract(entity=entity)

		entity.assertTrue(condition=self.parent is not None, message="Expression must be in a nested context.")

	def visitNestedEntities(self, entity: Nested, result: None) -> None:

		self.validateContract(entity=entity)

		interfaceCategories = {nested.category for nested in entity.interface}
		configCategories = {nested.category for nested in entity.configRaw}
		compositionCategories = {nested.category for nested in entity.composition}

		if entity.type == "interface":
			entity.assertTrue(condition=entity.isName, message="Interfaces must have a valid name.")
			entity.assertTrue(condition=interfaceCategories.issubset({"method", "expression"}),
				message="Interfaces can only expose methods and expressions as interface.")
			entity.assertTrue(condition=configCategories.issubset({"expression"}),
				message="Interfaces configuration can only contain expressions.")
			entity.assertTrue(condition=len(compositionCategories) == 0,
				message="Interfaces cannot have nested composition.")

		elif entity.type == "struct":
			entity.assertTrue(condition=entity.isName, message="Structures must have a valid name.")
			entity.assertTrue(condition=interfaceCategories.issubset({"expression", "enum", "nested", "using"}),
				message="Structures only accept expressions, enums, nested structs or using statements.")
			entity.assertTrue(condition=len(configCategories) == 0, message="Structures cannot have configuration.")
			entity.assertTrue(condition=len(compositionCategories) == 0,
				message="Structures cannot have nested composition.")

		elif entity.type == "component":
			entity.assertTrue(condition=entity.isName, message="Components must have a valid name.")
			entity.assertTrue(condition=interfaceCategories.issubset({"method"}),
				message="Components can only expose methods as interface.")
			entity.assertTrue(condition=configCategories.issubset({"expression"}),
				message="Components configuration can only contain expressions.")
			entity.assertTrue(condition=compositionCategories.issubset({"expression"}),
				message="Components composition can only contain expressions.")

		elif entity.type == "composition":
			entity.assertTrue(condition=not entity.hasInheritance, message="Compositions cannot have inheritance.")
			entity.assertTrue(condition=interfaceCategories.issubset({"expression"}),
				message="Compositions can only contain expressions.")
			entity.assertTrue(condition=len(configCategories) == 0, message="Compositions cannot have configuration.")
			entity.assertTrue(condition=len(compositionCategories) == 0,
				message="Compositions cannot have nested composition.")

		else:
			entity.assertTrue(condition=False, message="Unsupported nested type: '{}'.".format(entity.type))

	def visitMethod(self, entity: Method, result: None) -> None:

		self.validateContract(entity=entity)

		# Arguments
		argumentNames = set()
		for arg in entity.parameters:
			arg.assertTrue(condition=arg.isName, message="Argument is missing a name.")
			arg.assertTrue(condition=(arg.name not in argumentNames),
				message="Argument name '{}' is already defined.".format(arg.name))
			argumentNames.add(arg.name)
			arg.contracts.validate()

	def visitUsing(self, entity: Using, result: None) -> None:

		self.validateContract(entity=entity)

	def visitEnum(self, entity: Enum, result: None) -> None:

		self.validateContract(entity=entity)
