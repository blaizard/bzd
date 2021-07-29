import typing
from functools import cached_property

from bzd.parser.element import Element
from bzd.parser.error import Error
from bzd.validation.validation import Validation

from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.entity import Entity, Role


class Method(Entity):
	"""
	A method is a definition of a functional object.
	- Attributes:
		- name: The name of the method.
		- [type]: The return type if any.
	- Sequence:
		- argument: The list of arguments to this method.
	"""

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Type)
		Error.assertHasAttr(element=element, attr="name")

	@property
	def category(self) -> str:
		return "method"

	@property
	def isType(self) -> bool:
		return self.element.isAttr("type")

	@cached_property
	def type(self) -> typing.Optional[Type]:
		return Type(element=self.element, kind="type", template="template") if self.isType else None

	@cached_property
	def validation(self) -> typing.Optional[Validation]:
		"""
		Generate a validation based on the argument list.
		"""
		schema = {}
		for arg in self.args:
			validation = []
			# Arguments declared with no value are mandatory, others are just optional.
			if not arg.isArg:
				validation.append("mandatory")
			schema[arg.name] = " ".join(validation)
		return Validation(schema=schema)

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve entities.
		"""
		# Generate this symbol FQN
		fqn = symbols.makeFQN(name=self.name, namespace=namespace)
		self._setUnderlyingType(fqn)

		maybeType = self.type
		if maybeType is not None:
			maybeType.resolve(symbols=symbols, namespace=namespace, exclude=exclude)
		for arg in self.args:
			arg.type.resolve(symbols=symbols, namespace=namespace, exclude=exclude)

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")

	@cached_property
	def args(self) -> typing.List[Expression]:
		if self.element.isNestedSequence("argument"):
			sequence = self.element.getNestedSequence("argument")
			assert sequence is not None
			return [Expression(element=arg) for arg in sequence]
		return []

	def __repr__(self) -> str:
		return self.toString({"name": self.name})
