import typing
import enum
from functools import cached_property

from bzd.parser.element import Element, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

from tools.bdl.contracts.validation import Validation, SchemaDict
from tools.bdl.contracts.contract import Contract
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.fragment.contract import Contracts
from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.entity import Entity, EntityExpression, Role
from tools.bdl.entities.impl.types import Category
from tools.bdl.entities.impl.fragment.expression_fragment import ExpressionFragment

if typing.TYPE_CHECKING:
	from tools.bdl.visitors.symbol_map import Resolver


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
		- argument: The list of arguments to pass to the instanciation or method call.
		- argument_resolved: List of resolved arguments.
	"""

	def __init__(self, element: Element) -> None:

		super().__init__(element, Role.Value)
		#self.assertTrue(condition=(self.isValue or self.isSymbol),
		#	message="Expression must represent a symbol or a value.")

	@cached_property
	def typeResolved(self) -> Symbol:
		return Symbol(element=self.element,
			kind="symbol",
			underlyingTypeFQN="fqn_type",
			template="template_resolved",
			const="const",
			contract="contract")

	@property
	def isInterfaceType(self) -> bool:
		return self.element.isAttr("interface") and bool(self.element.getAttr("interface").value)

	@cached_property
	def interfaceType(self) -> Symbol:
		return Symbol(element=self.element, kind="interface", underlyingTypeFQN="fqn_interface",
			const="const") if self.isInterfaceType else self.symbol

	@cached_property
	def interfaceTypeResolved(self) -> Symbol:
		return Symbol(element=self.element, kind="interface", underlyingTypeFQN="fqn_interface",
			const="const") if self.isInterfaceType else self.typeResolved

	@property
	def fragments(self) -> typing.List[ExpressionFragment]:
		"""Return a list with all fragments."""

		return [ExpressionFragment.make(element) for element in self.element.getNestedSequenceOrEmpty("fragments")]

	@property
	def dependencies(self) -> typing.Set[str]:
		"""
		Output the dependency list for this entity.
		"""
		dependencies = set()
		if self.isSymbol:
			dependencies.update(self.symbol.dependencies)
		if self.isParameters:
			assert self.parameters is not None
			dependencies.update(self.parameters.dependencies)

		return dependencies

	def processFragments(self, resolver: "Resolver") -> None:
		"""Process fragments to build a value or a symbol."""

		self.assertTrue(condition=self.element.isNestedSequence("fragments"), message=f"Missing nested sequence 'fragment' for: {self.element}")

		self.assertTrue(condition=len(self.fragments) == 1, message=f"This expression must have a single fragment.")
		for fragment in self.fragments:
			fragment.resolve(resolver=resolver)

		fragment.toElement(self.element)

	def resolve(self, resolver: "Resolver") -> None:
		"""Resolve entities."""

		# Resolve the interface associated with this expression.
		if self.isInterfaceType:
			self.interfaceType.resolve(resolver=resolver)
			# TODO: Ensure that the interface is part of the parent type.

		self.processFragments(resolver=resolver)

		# If it holds a value, it is considered a literal.
		if self.isValue:
			pass

		# If it holds a symbol.
		elif self.isSymbol:

			# Set the executor.
			entity = self.symbol.getEntityResolved(resolver=resolver)
			executorContract = self.contracts.get("executor")
			if executorContract is not None:
				validExecutorContract = entity.category == Category.component
				validExecutorContract |= (entity.category == Category.method) and not self.symbol.isThis
				self.assertTrue(condition=validExecutorContract,
					message=
					"`executor` contracts must be set either at component instantiation or at free function call.")
				executor = executorContract.value
			# If there is a 'this', propagate the executor.
			elif self.symbol.isThis:
				executor = self.symbol.getThisResolved(resolver=resolver).executor
			else:
				executor = None
			if executor is not None:
				self._setExecutor(executor)

		else:
			self.error(message="Unsupported expression.")

		super().resolve(resolver)

		# Validate the whole expression with the contracts if any.
		if self.contracts.validationForValue:
			validation = Validation(schema=[self.contracts.validationForValue], args={"resolver": resolver})
			result = validation.validate([self], output="return")
			self.assertTrue(condition=bool(result), message=str(result))

	@cached_property
	def parameters(self) -> typing.Optional[Parameters]:
		"""
		Return the Parameters object if there are parameters. In case the expression
		is declared with empty parenthesis or without the Parameters object will be empty.
		"""
		if self.isParameters:
			return Parameters(element=self.element, NestedElementType=Expression, nestedKind="argument")
		return Parameters(element=self.element, NestedElementType=Expression)

	@cached_property
	def parametersResolved(self) -> ParametersResolved:
		return ParametersResolved(element=self.element,
			NestedElementType=Expression,
			param="argument_resolved",
			expected="argument_expected")
