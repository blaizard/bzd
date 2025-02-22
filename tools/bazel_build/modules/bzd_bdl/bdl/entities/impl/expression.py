import typing
import enum
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

from bdl.contracts.validation import Validation, SchemaDict
from bdl.contracts.contract import Contract
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.contract import Contracts
from bdl.entities.impl.fragment.parameters import Parameters
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.entity import Entity, EntityExpression, Role
from bdl.entities.impl.types import Category
from bdl.entities.impl.fragment.expression_fragment import (
    ExpressionFragment,
    OperatorFragment,
)

if typing.TYPE_CHECKING:
	from bdl.visitors.symbol_map import Resolver, SymbolMap


class Expression(EntityExpression):
	"""
    An expression can be:
            - a variable declaration.
            - a variable definition.
            - a method call.
            - a value.
    - Attributes:
            - [type]: The method name or variable type to be used.
            - [value]: The value this expression represents.
            - [name]: The resulting symbol name.
            - [const]: If the expression is constant.
            - [underlyingValueFQN]: The actual value discovered after resolution.
            - [symbol]: The interface type if any (might not be set).
            - [executor]: The executor for which this expression should run.
    - Sequence:
            - argument: The list of arguments to pass to the instantiation or method call.
            - argument_resolved: List of resolved arguments.
    """

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Value)
		# self.assertTrue(condition=(self.isValue or self.isSymbol),
		# 	message="Expression must represent a symbol or a value.")

	@property
	def typeResolved(self) -> Symbol:
		return Symbol(
		    element=self.element,
		    kind="symbol",
		    underlyingTypeFQN="fqn_type",
		    template="template_resolved",
		    const="const",
		    contract="contract",
		)

	@property
	def isInterfaceType(self) -> bool:
		return self.element.isAttr("interface") and bool(self.element.getAttr("interface").value)

	@property
	def interfaceType(self) -> Symbol:
		return (Symbol(
		    element=self.element,
		    kind="interface",
		    underlyingTypeFQN="fqn_interface",
		    const="const",
		) if self.isInterfaceType else self.symbol)

	@property
	def interfaceTypeResolved(self) -> Symbol:
		return (Symbol(
		    element=self.element,
		    kind="interface",
		    underlyingTypeFQN="fqn_interface",
		    const="const",
		) if self.isInterfaceType else self.typeResolved)

	@property
	def fragments(self) -> typing.List[ExpressionFragment]:
		"""Return a list with all fragments."""

		return [ExpressionFragment.make(element) for element in self.element.getNestedSequenceOrEmpty("fragments")]

	@property
	def dependencies(self) -> typing.Set[str]:
		"""Output the dependency list for this entity."""

		dependencies = set()
		if self.isSymbol:
			dependencies.update(self.symbol.dependencies)
			dependencies.update(self.parametersResolved.dependencies)

		return dependencies

	@property
	def this(self) -> typing.Optional[str]:
		"""Get the fqn of 'this' from this expression if available."""

		return self.symbol.this if self.isSymbol and self.symbol.isThis else None

	def makeResolver(self, symbols: "SymbolMap", **kwargs: typing.Any) -> "Resolver":
		"""Create a resolver for this expression."""

		if self.isFQN:
			return symbols.makeResolver(namespace=self.namespace, this=self.this, **kwargs)
		return symbols.makeResolver(this=self.this, **kwargs)

	def resolveFragments(self, resolver: "Resolver") -> None:
		"""Process fragments to build a value or a symbol."""

		self.assertTrue(
		    condition=self.element.isNestedSequence("fragments"),
		    message=f"Missing nested sequence 'fragments' for: {self.element}",
		)

		# Resolve all fragments
		fragments = self.fragments
		for fragment in fragments:
			fragment.resolve(resolver=resolver)

		operatorsPrecedence = [
		    ("unary", {"-", "+"}),
		    ("binary", {"*", "/"}),
		    ("binary", {"+", "-"}),
		]

		for kind, operators in operatorsPrecedence:
			fragmentPrevious: typing.Optional[ExpressionFragment] = None
			index = 0
			while index < len(fragments):
				fragment = fragments[index]
				fragmentNext: typing.Optional[ExpressionFragment] = (fragments[index + 1] if index +
				                                                     1 < len(fragments) else None)
				if (isinstance(fragment, OperatorFragment) and fragment.operator in operators):
					# Look for operators that are preceded with another operator or at the beginning.
					isUnary = kind == "unary"
					isUnary &= fragmentPrevious is None or isinstance(fragmentPrevious, OperatorFragment)
					isUnary &= fragmentNext is not None and not isinstance(fragmentNext, OperatorFragment)
					if isUnary:
						assert fragmentNext
						fragmentNext.unary(fragment)
						del fragments[index]
						index += 1
						fragmentPrevious = fragment
						continue

					# Look for operators that are between 2 non-operators.
					isBinary = kind == "binary"
					isBinary &= fragmentPrevious is not None and not isinstance(fragmentPrevious, OperatorFragment)
					isBinary &= fragmentNext is not None and not isinstance(fragmentNext, OperatorFragment)
					if isBinary:
						assert fragmentPrevious
						assert fragmentNext
						fragmentPrevious.binary(fragment, fragmentNext)
						del fragments[index:index + 2]
						continue

				index += 1
				fragmentPrevious = fragment

		self.assertTrue(
		    condition=len(fragments) == 1,
		    message=f"This expression is malformed: {fragments}",
		)
		fragments[0].toElement(self.element)

		# TODO: If mark as "mandatory", remove the default value.

	def resolve(self, resolver: "Resolver") -> None:
		"""Resolve entities.

        Args:
            resolver: The resolver to be used.
        """

		# Resolve the interface associated with this expression.
		if self.isInterfaceType:
			self.interfaceType.resolve(resolver=resolver)
			# TODO: Ensure that the interface is part of the parent type.

		self.resolveFragments(resolver=resolver)
		self.assertTrue(
		    condition=self.isValue or self.isSymbol or self.isRegexpr,
		    message=f"Unsupported expression: {self.element}",
		)

		super().resolve(resolver)

		# Validate the whole expression with the contracts if any.
		if self.contracts.validationForValue:
			validation = Validation(schema=[self.contracts.validationForValue], args={"resolver": resolver})
			result = validation.validate([self], output="return")
			self.assertTrue(condition=bool(result),
			                message=f"While validating '{self.contracts.validationForValue}': {result}")

	@property
	def parameters(self) -> Parameters:
		"""
        Return the Parameters object if there are parameters. In case the expression
        is declared with empty parenthesis or without the Parameters object will be empty.
        """
		if self.isParameters:
			return Parameters(
			    element=self.element,
			    NestedElementType=Expression,
			    nestedKind="argument",
			)
		return Parameters(element=self.element, NestedElementType=Expression)

	@property
	def parametersResolved(self) -> ParametersResolved:
		return ParametersResolved(
		    element=self.element,
		    NestedElementType=Expression,
		    param="argument_resolved",
		    expected="argument_expected",
		)
