import enum
"""File containing global types to share between entities."""


# Types matching the category of an entity.
class Category(enum.Enum):
	struct = "struct"
	component = "component"
	interface = "interface"
	composition = "composition"
	builtin = "builtin"
	enum = "enum"
	enumValue = "enumValue"
	method = "method"
	namespace = "namespace"
	using = "using"
	use = "use"
	extern = "extern"
	expression = "expression"
