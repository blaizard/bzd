import typing
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from bdl.entities.impl.fragment.contract import Contracts
from bdl.entities.impl.entity import EntityExpression, Role


class Using(EntityExpression):
	"""
    Using statement are defining a type based on an underlying type.
    - Attributes:
            - name: (optional) The name of the new type, the one created by this entity.
            - type: The underlying type.
    """

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="symbol")

	def resolve(self, resolver: typing.Any) -> None:
		"""
        Resolve entities.
        """
		entity = self.symbol.resolve(resolver=resolver)

		if entity.isRoleMeta:
			self._setMeta()

		super().resolve(resolver)

	def __repr__(self) -> str:
		return self.toString({
		    "name": self.name if self.isName else None,
		    "symbol": self.symbol.name,
		    "meta": "True" if self.isRoleMeta else None,
		})
