import typing
import pathlib

from bzd.parser.element import Element
from bzd.parser.visitor import Visitor
from bzd.parser.error import Error

from bzd.template2.parser import Parser

SubstitutionsType = typing.Dict[str, typing.Any]

_defaultValue: typing.Any = {}


class VisitorTemplate(Visitor[str, str]):
	nestedKind = None

	def __init__(self, substitutions: SubstitutionsType) -> None:
		self.substitutions = substitutions

	def visitBegin(self, result: str) -> str:
		return ""

	def resolveName(self, substitutions: SubstitutionsType, name: str,
		argument: typing.Any = _defaultValue) -> typing.Any:
		"""
		Resolve a name from the current substitution list.
		Names are splited at the '.' character and each symbol is matched with the current
		substitution list.
		If this is a callable, it will be resolved.
		"""
		value: typing.Any = substitutions

		for symbol in name.split("."):

			# Look for the corresponding nested value
			if hasattr(value, symbol):
				value = getattr(value, symbol)
			elif symbol in value:
				value = value[symbol]
			else:
				raise Exception("Substitution value for '{}' does not exists.".format(name))

			# If this is a callable, process its value
			if argument is not _defaultValue:
				assert callable(value), "'{}' must be callable.".format(name)
				value = value(argument)
			elif callable(value):
				value = value()

		return value

	def visitSubstitution(self, element: Element) -> str:
		"""
		Handle substitution.
		"""
		Error.assertHasAttr(element=element, attr="name")
		value = self.resolveName(substitutions=self.substitutions, name=element.getAttr("name").value)

		# Process the pipes if any.
		pipes = element.getNestedSequence("pipe")
		if pipes:
			# Note, element is used on purpose here to ensure a more accurate error reporting.
			for element in pipes:
				Error.assertHasAttr(element=element, attr="name")
				value = self.resolveName(substitutions=self.substitutions,
					name=element.getAttr("name").value,
					argument=value)

		# Save the output
		assert isinstance(value,
			(int, float, str)), "The resulting substitued value must be a number or a string."
		return str(value)

	def visitForBlock(self, element: Element) -> str:
		"""
		Handle for loop block.
		"""

		Error.assertHasAttr(element=element, attr="value1")
		Error.assertHasAttr(element=element, attr="iterable")
		Error.assertHasSequence(element=element, sequence="nested")

		value1 = element.getAttr("value1").value
		Error.assertTrue(element=element, condition=value1 not in self.substitutions, message="Name conflict, '{}' already exists in the substitution map.".format(value1))

		value2 = element.getAttrValue("value2")
		Error.assertTrue(element=element, condition=(value2 is None) or (value2 not in self.substitutions), message="Name conflict, '{}' already exists in the substitution map.".format(value2))

		sequence = element.getNestedSequence(kind="nested")
		assert sequence

		# Loop through the elements
		result = ""
		iterable = self.resolveName(substitutions=self.substitutions, name=element.getAttr("iterable").value)
		if value2 is None:
			for value in iterable:
				self.substitutions[value1] = value
				result += self._visit(sequence = sequence)
				del self.substitutions[value1]
		else:
			iterablePair: typing.Iterator = iterable.items() if isinstance(iterable, dict) else enumerate(iterable)
			for key, value in iterablePair:
				self.substitutions[value1] = key
				self.substitutions[value2] = value
				result += self._visit(sequence = sequence)
				del self.substitutions[value2]
				del self.substitutions[value1]

		return result

	def visitElement(self, element: Element, result: str) -> str:
		"""
		Go through all elements and dispatch the action.
		"""

		Error.assertHasAttr(element=element, attr="category")
		category = element.getAttr("category").value

		try:

			# Raw content
			if category == "content":
				Error.assertHasAttr(element=element, attr="content")
				result += element.getAttr("content").value

			# Substitution
			elif category == "substitution":
				result += self.visitSubstitution(element = element)

			# For loop block
			elif category == "for":
				result += self.visitForBlock(element = element)

			else:
				raise Exception("Unsupported category: '{}'.".format(category))

		except Exception as e:
			Error.handleFromElement(element=element, message=str(e))

		return result


class Template:

	def __init__(
			self,
			template: str,
			includeDirs: typing.Sequence[pathlib.Path] = [pathlib.Path(__file__).parent.parent.parent.parent]) -> None:
		self.template = template
		self.includeDirs = includeDirs

	def process(self, substitutions: SubstitutionsType) -> str:

		sequence = Parser(self.template).parse()

		print(sequence)

		result = VisitorTemplate(substitutions).visit(sequence)

		print(result)

		return result
