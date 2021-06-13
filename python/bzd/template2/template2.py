import typing
import pathlib

from bzd.parser.element import Element
from bzd.parser.visitor import Visitor
from bzd.parser.error import Error

from bzd.template2.parser import Parser

SubstitutionsType = typing.Dict[str, typing.Any]

_defaultValue: typing.Any = {}


class VisitorTemplate(Visitor[str, str]):
	nestedKind: None

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

	def visitElement(self, element: Element, result: str) -> str:

		Error.assertHasAttr(element=element, attr="category")
		category = element.getAttr("category").value

		try:

			if category == "content":

				Error.assertHasAttr(element=element, attr="content")
				result += element.getAttr("content").value

			elif category == "substitution":

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
				result += str(value)

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
