import typing
import json
from functools import cached_property

from bzd.parser.element import Element, Sequence, ElementBuilder
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor as VisitorBase
from bdl.entities.impl.fragment.parameters import Parameters

from bdl.entities.impl.expression import Expression
from bdl.entities.impl.entity import Entity, EntityExpression, Role
from bdl.entities.impl.fragment.fqn import FQN


class EnumValue(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Value)
		Error.assertHasAttr(element=element, attr="name")

	def resolve(self, resolver: typing.Any) -> None:
		"""Resolve entities."""

		# Generate this symbol FQN
		underlyingTypeFQN = FQN.fromNamespace(FQN.toNamespace(self.fqn)[:-1])
		self._setUnderlyingTypeFQN(underlyingTypeFQN)
		self._setLiteral({"type": "enum", "fqn": self.fqn})
		super().resolve(resolver)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	def __repr__(self) -> str:
		return self.toString({"name": self.name})


class Enum(Entity):
	"""
    Enumerates available values for a type.
    - Attributes:
            - name: The name of the enum.
    - Sequence:
            - values: The different values.
    """

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasSequence(element=element, sequence="values")

	def resolve(self, resolver: typing.Any) -> None:
		"""Resolve entities."""

		self._setUnderlyingTypeFQN(self.fqn)
		super().resolve(resolver)

	def toLiteral(self, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:

		assert "value" in args, f"There must be a value argument: {args}."
		value = args['value']
		assert isinstance(value, EntityExpression), f"The value must be of type 'EntityExpression', not {value}."
		assert value.isLiteral, f"It must have a literal type: {value}."
		literal = value.literalNative
		assert isinstance(literal,
		                  dict) and literal.get("type") == "enum", f"The value must be of type 'enum', not '{literal}'."
		return value.literal

	def getConfigValues(self, resolver: "Resolver") -> Parameters:

		# Get the default value.
		maybeFirst = next(iter(self.values), None)
		assert maybeFirst, f"This enum '{self.fqn}' does not have any values: '{self.values}'"
		literal = json.dumps({"type": "enum", "fqn": maybeFirst.fqn})

		# Create the parameter.
		params = Parameters(element=self.element, NestedElementType=Expression)
		element = ElementBuilder().setAttr("category", "expression").setAttr("name", "value").setAttr(
		    "literal", literal).setAttr("symbol", maybeFirst.fqn)
		expression = Expression(element)
		params.append(expression, order=0)

		return params

	@property
	def values(self) -> typing.Iterable[EnumValue]:
		sequence = self.element.getNestedSequence("values")
		assert sequence is not None
		return [EnumValue(element) for element in sequence]

	def __repr__(self) -> str:
		return self.toString({"name": self.name, "values": ", ".join([e.name for e in self.values])})
