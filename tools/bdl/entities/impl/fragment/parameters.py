import typing
from functools import cached_property
from dataclasses import dataclass

from bzd.parser.element import Element, Sequence, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

if typing.TYPE_CHECKING:
	from tools.bdl.entities.impl.expression import Expression
	from tools.bdl.visitors.symbol_map import SymbolMap
	from tools.bdl.entities.impl.fragment.type import Type
	from tools.bdl.entities.impl.entity import Entity

ResolvedType = typing.Union[str, "Entity", "Type"]

T = typing.TypeVar("T")


class ParametersCommon(typing.Generic[T]):

	def __init__(self, element: Element) -> None:
		self.element = element
		self.list: typing.List[typing.Tuple["Expression", T]] = []

	def __iter__(self) -> typing.Iterator["Expression"]:
		for parameter in self.list:
			yield parameter[0]

	def __getitem__(self, index: int) -> "Expression":
		return self.list[index][0]

	def __len__(self) -> int:
		return len(self.list)

	def __bool__(self) -> bool:
		return bool(self.list)

	def empty(self) -> bool:
		return not bool(self.list)

	def size(self) -> int:
		return len(self.list)

	def at(self, index: int) -> "Expression":
		return self.list[index][0]

	def append(self, entity: "Expression", metadata: T) -> None:
		self.list.append((entity, metadata))

	@staticmethod
	def makeKey(entity: "Expression", index: int) -> str:
		return entity.name if entity.isName else str(index)

	def items(self) -> typing.Iterator[typing.Tuple[str, "Expression"]]:
		for index, data in enumerate(self.list):
			name = ParametersCommon.makeKey(data[0], index)
			yield name, data[0]

	def itemsMetadata(self) -> typing.Iterator[typing.Tuple[str, "Expression", T]]:
		for index, data in enumerate(self.list):
			name = ParametersCommon.makeKey(data[0], index)
			yield name, data[0], data[1]

	def keys(self) -> typing.Iterator[str]:
		for index, data in enumerate(self.list):
			yield ParametersCommon.makeKey(data[0], index)

	def contains(self, name: str) -> bool:
		"""
		Wether or not a named parameters contains the entry.
		"""
		for key, _ in self.items():
			if key == name:
				return True
		return False

	def atKeyMetadata(self, name: str) -> T:
		"""
		Return the metadata at a specific key index.
		"""
		for key, _, metadata in self.itemsMetadata():
			if key == name:
				return metadata
		raise KeyError("Missing key '{}'.".format(name))

	def __repr__(self) -> str:
		content = []
		for key, expression, metadata in self.itemsMetadata():
			content.append("{}: {} {}".format(key, str(expression.element), str(metadata)))
		return "\n".join(content)


@dataclass
class Metadata:
	default: bool = False
	order: int = -1
	template: bool = False


class Parameters(ParametersCommon[Metadata]):
	"""
    Describes the parameter list, a collection of expression.
    """

	def __init__(self,
		element: Element,
		nestedKind: typing.Optional[str] = None,
		filterFct: typing.Optional[typing.Callable[["Expression"], bool]] = None) -> None:

		super().__init__(element=element)

		if nestedKind:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from tools.bdl.entities.impl.expression import Expression
				for index, e in enumerate(sequence):
					expression = Expression(e)
					if filterFct is not None and not filterFct(expression):
						continue
					self.append(expression, Metadata(order=index, template=expression.contracts.has("template")))
					expression.assertTrue(condition=self.isNamed == expression.isName,
						message="Cannot mix named and unnamed parameters: '{}' and '{}'.".format(
						self.at(0), expression))
					expression.assertTrue(condition=not expression.isVarArgs or index == len(sequence) - 1,
						message="Variable arguments can only be present at the end of the parameter list.")

		# Sanity check.
		if self.isNamed is True:
			assert not self.isVarArgs, "Cannot have named and varargs."

	def copy(self, template: typing.Optional[bool] = None) -> "Parameters":
		"""
		Copy the parameter list and optionally filter it.
		"""
		parameters = Parameters(element=self.element)

		# Fill in the list
		for key, expression, metadata in self.itemsMetadata():
			if template is not None and template != metadata.template:
				continue
			parameters.append(expression, metadata)

		return parameters

	@property
	def isNamed(self) -> typing.Optional[bool]:
		"""
		Wether or not a parameter pack contains named parameters or not.
		Returns None if it contains no parameters.
		"""
		if not self.empty():
			return self.at(0).isName
		return None

	@property
	def isVarArgs(self) -> typing.Optional[bool]:
		"""
		Wether or not the last parameter is a var args.
		"""
		if not self.empty():
			return self.at(-1).isVarArgs
		return None

	def mergeDefaults(self, parameters: "Parameters") -> None:
		"""
		Merge default parameters with this.
		"""
		if not bool(parameters):
			return
		if bool(self):
			if self.isNamed != parameters.isNamed:
				Error.assertTrue(element=self.element,
					condition=bool(self.isNamed),
					message="Requires named parameters.")
				Error.assertTrue(element=self.element,
					condition=not bool(self.isNamed),
					message="Requires unnamed parameters.")

		# Merge the values
		order: int = 0
		for name, expression, metadata in parameters.itemsMetadata():
			if not self.contains(name):
				expression.assertTrue(condition=not expression.contracts.has("mandatory"),
					message="Missing mandatory parameter: '{}'.".format(name))
				self.append(expression, Metadata(default=True))
			# Merge the metadata
			self.atKeyMetadata(name).template = metadata.template
			self.atKeyMetadata(name).order = order
			order += 1

	def resolve(self,
		symbols: "SymbolMap",
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
        Resolve all parameters.
        """

		for parameter in self:
			parameter.resolveMemoized(symbols=symbols, namespace=namespace, exclude=exclude)

	def itemsValuesOrTypes(
			self, symbols: "SymbolMap",
			exclude: typing.Optional[typing.List[str]]) -> typing.List[typing.Tuple[str, ResolvedType, Metadata]]:
		"""
        Iterate through the list and return values or types.
        """

		values: typing.List[typing.Tuple[str, ResolvedType, Metadata]] = []

		for key, expression, metadata in self.itemsMetadata():

			if expression.literal is not None:
				values.append((key, expression.literal, metadata))

			elif expression.underlyingValue is not None:
				value: ResolvedType = symbols.getEntityResolved(fqn=expression.underlyingValue).assertValue(
					element=expression.element)

				# If these are default arguments, use the default value.
				if metadata.default:
					from tools.bdl.entities.impl.expression import Expression
					assert isinstance(value, Expression)
					if value.literal:
						# I beleive that this code path is never used.
						value = value.literal
					elif value.isName:
						# Create an unamed value
						element = ElementBuilder.cast(value.element.copy(), ElementBuilder).removeAttr(key="name").get()
						value = Expression(element)

				values.append((key, value, metadata))

			else:
				values.append((key, expression.type, metadata))

		return values

	def getValuesOrTypesAsDict(self, symbols: "SymbolMap", exclude: typing.Optional[typing.List[str]]) -> typing.Dict[
		str, ResolvedType]:
		"""
        Get the values as a dictionary.
        """
		values = self.itemsValuesOrTypes(symbols=symbols, exclude=exclude)
		return {entry[0]: entry[1] for entry in values}

	def toResolvedSequence(self,
		symbols: "SymbolMap",
		exclude: typing.Optional[typing.List[str]],
		onlyTypes: bool = False,
		onlyValues: bool = False) -> Sequence:
		"""
		Build the resolved sequence of those parameters.
		Must be called after mergeDefaults.
		"""
		sequence = SequenceBuilder()

		entries = self.itemsValuesOrTypes(symbols=symbols, exclude=exclude)
		if self.isNamed:
			entries = sorted(entries, key=lambda k: k[2].order)

		# Build the sequence
		from tools.bdl.entities.impl.fragment.type import Type
		for key, item, metadata in entries:
			if onlyTypes:
				Error.assertTrue(element=self.element,
					condition=isinstance(item, Type),
					message="Parameter '{}' is not a type.".format(key))
			if onlyValues:
				Error.assertTrue(element=self.element,
					condition=not isinstance(item, Type),
					message="Parameter '{}' is not a value.".format(key))
			if isinstance(item, str):
				element = ElementBuilder().setAttr(key="value", value=item)
			else:
				element = item.element.copy()
			if self.isNamed:
				ElementBuilder.cast(element, ElementBuilder).setAttr(key="key", value=key)
			sequence.pushBackElement(element)

		return sequence


class ResolvedParameters(ParametersCommon[Metadata]):

	def __init__(self, element: Element, nestedKind: typing.Optional[str]) -> None:

		super().__init__(element=element)

		if nestedKind is not None:
			sequence = self.element.getNestedSequence(nestedKind)
			if sequence:
				from tools.bdl.entities.impl.expression import Expression
				for index, e in enumerate(sequence):
					self.append(Expression(e), Metadata())

	@property
	def isNamed(self) -> typing.Optional[bool]:
		return False

	@property
	def isVarArgs(self) -> typing.Optional[bool]:
		return False
