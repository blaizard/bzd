import typing
from functools import cached_property

from bzd.parser.element import Element, Sequence
from bzd.parser.error import Error
from bzd.parser.visitor import Visitor as VisitorBase

from bdl.entities.impl.entity import Entity, Role
from bdl.entities.impl.fragment.fqn import FQN


class EnumValue(Entity):

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Value)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")


class _Visitor(VisitorBase[str, typing.List[str]]):
	nestedKind = None

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		Error.assertHasAttr(element=element, attr="name")
		result.append(element.getAttr("name").value)
		return result


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
		"""
        Resolve entities.
        """
		# Generate this symbol FQN
		self._setUnderlyingTypeFQN(self.fqn)
		super().resolve(resolver)

	@property
	def values(self) -> typing.Iterable[EnumValue]:
		sequence = self.element.getNestedSequence("values")
		assert sequence is not None
		return [EnumValue(element) for element in sequence]

	def __repr__(self) -> str:
		return self.toString({"name": self.name, "values": ", ".join([e.name for e in self.values])})
