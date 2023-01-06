from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use, EntityType
from tools.bdl.entities.impl.types import Category


class Validation(VisitorBase[None]):

	def validateContract(self, entity: EntityType) -> None:

		entity.contracts.validate()

	def visitExpression(self, entity: Expression, result: None) -> None:

		self.validateContract(entity=entity)

		entity.assertTrue(condition=self.parent is not None, message="Expression must be in a nested context.")

	def visitNestedEntities(self, entity: Nested, result: None) -> None:

		self.validateContract(entity=entity)

		interfaceCategories = {nested.category for nested in entity.interface}
		configCategories = {nested.category for nested in entity.configRaw}
		compositionCategories = {nested.category for nested in entity.composition}

		if entity.category == Category.interface:
			entity.assertTrue(condition=entity.isName, message="Interfaces must have a valid name.")
			entity.assertTrue(condition=interfaceCategories.issubset({Category.method, Category.expression}),
				message="Interfaces can only expose methods and expressions as interface.")
			entity.assertTrue(condition=configCategories.issubset({Category.expression, Category.using}),
				message="Interfaces configuration can only contain expressions and using statements.")
			entity.assertTrue(condition=len(compositionCategories) == 0,
				message="Interfaces cannot have nested composition.")

		elif entity.category == Category.struct:
			entity.assertTrue(condition=entity.isName, message="Structures must have a valid name.")
			entity.assertTrue(condition=interfaceCategories.issubset(
				{Category.expression, Category.enum, Category.struct, Category.using}),
				message="Structures only accept expressions, enums, nested structs or using statements.")
			entity.assertTrue(condition=len(configCategories) == 0, message="Structures cannot have configuration.")
			entity.assertTrue(condition=len(compositionCategories) == 0,
				message="Structures cannot have nested composition.")

		elif entity.category == Category.component:
			entity.assertTrue(condition=entity.isName, message="Components must have a valid name.")
			entity.assertTrue(condition=interfaceCategories.issubset({Category.method, Category.expression}),
				message="Components can only expose methods and expressions as interface.")
			entity.assertTrue(condition=configCategories.issubset({Category.expression, Category.using}),
				message="Components configuration can only contain expressions or using statements.")
			entity.assertTrue(condition=compositionCategories.issubset({Category.expression}),
				message="Components composition can only contain expressions.")

		elif entity.category == Category.composition:
			entity.assertTrue(condition=not entity.hasInheritance, message="Compositions cannot have inheritance.")
			entity.assertTrue(condition=interfaceCategories.issubset({Category.expression}),
				message="Compositions can only contain expressions.")
			entity.assertTrue(condition=len(configCategories) == 0, message="Compositions cannot have configuration.")
			entity.assertTrue(condition=len(compositionCategories) == 0,
				message="Compositions cannot have nested composition.")

		else:
			entity.assertTrue(condition=False, message="Unsupported nested category: '{}'.".format(entity.category))

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
