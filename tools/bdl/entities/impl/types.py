import enum
"""File containing global types to share between entities."""


# Types matching the type category of an entity.
class TypeCategory(enum.Enum):
	struct = "struct"
	component = "component"
	interface = "interface"
	composition = "composition"
	builtin = "builtin"
	enum = "enum"
	method = "method"
	namespace = "namespace"
	using = "using"
