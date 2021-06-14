import typing
import pathlib
import re

from bzd.parser.element import Element
from bzd.parser.visitor import Visitor
from bzd.parser.error import Error, ExceptionParser

from bzd.template2.parser import Parser

SubstitutionsType = typing.Dict[str, typing.Any]

_defaultValue: typing.Any = {}

class VisitorTemplate(Visitor[str, str]):
	nestedKind = None

	def __init__(self, substitutions: SubstitutionsType) -> None:
		self.substitutions = substitutions
		# Used to trigger the else condition, this works as any un-resolved if must be followed by the else (if any).
		self.followElse = False

	def visitBegin(self, result: str) -> str:
		return ""

	def resolveName(self, name: str, argument: typing.Any = _defaultValue) -> typing.Any:
		"""
		Resolve a name from the current substitution list.
		Names are splited at the '.' character and each symbol is matched with the current
		substitution list.
		If this is a callable, it will be resolved.
		"""
		value: typing.Any = self.substitutions

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
		value = self.resolveName(name=element.getAttr("name").value)

		# Process the pipes if any.
		pipes = element.getNestedSequence("pipe")
		if pipes:
			for pipe in pipes:
				Error.assertHasAttr(element=pipe, attr="name")
				value = self.resolveName(name=pipe.getAttr("name").value, argument=value)

		# Save the output
		assert isinstance(value, (int, float, str)), "The resulting substitued value must be a number or a string."
		return str(value)

	def visitForBlock(self, element: Element) -> str:
		"""
		Handle for loop block.
		"""

		Error.assertHasAttr(element=element, attr="value1")
		Error.assertHasAttr(element=element, attr="iterable")
		Error.assertHasSequence(element=element, sequence="nested")

		value1 = element.getAttr("value1").value
		Error.assertTrue(element=element,
			condition=value1 not in self.substitutions,
			message="Name conflict, '{}' already exists in the substitution map.".format(value1))

		value2 = element.getAttrValue("value2")
		Error.assertTrue(element=element,
			condition=(value2 is None) or (value2 not in self.substitutions),
			message="Name conflict, '{}' already exists in the substitution map.".format(value2))

		sequence = element.getNestedSequence(kind="nested")
		assert sequence

		# Loop through the elements
		result = ""
		iterable = self.resolveName(name=element.getAttr("iterable").value)
		if value2 is None:
			for value in iterable:
				self.substitutions[value1] = value
				result += self._visit(sequence=sequence)
				del self.substitutions[value1]
		else:
			iterablePair = iterable.items() if isinstance(iterable, dict) else enumerate(iterable)
			for key, value in iterablePair:
				self.substitutions[value1] = key
				self.substitutions[value2] = value
				result += self._visit(sequence=sequence)
				del self.substitutions[value2]
				del self.substitutions[value1]

		return result

	def visitIfBlock(self, element: Element, conditionStr: str) -> str:
		"""
		Handle if block.
		"""

		Error.assertHasSequence(element=element, sequence="nested")
		sequence = element.getNestedSequence(kind="nested")
		assert sequence

		result = ""
		condition = self.evaluateCondition(conditionStr=conditionStr)
		self.followElse = not condition
		if condition:
			result += self._visit(sequence=sequence)

		return result

	def evaluateCondition(self, conditionStr: str) -> bool:
		"""
		Resolve names and evaluate a condition statement.
		"""

		def replaceValue(match: typing.Match[str]) -> str:
			# Try to read the value, if it cannot, return the string as is.
			try:
				value = self.resolveName(name=match.group())
			except:
				return match.group()

			# Encode the value to string and evaluate it to boolean if needed.
			if isinstance(value, (bool, int, float)):
				return str(value)
			elif isinstance(value, str):
				return "\"{}\"".format(value)
			elif isinstance(value, (list, dict)):
				return str(bool(value))

			raise Exception("Unsupported type ({}) for value '{}'.".format(type(value), match.group()))

		patternWord = re.compile("[^\s]+")
		conditionStr = re.sub(patternWord, replaceValue, conditionStr)
		try:
			condition = eval(conditionStr)
		except:
			raise Exception("Cannot evaluate condition '{}'.".format(conditionStr))

		return bool(condition)

	def visitElement(self, element: Element, result: str) -> str:
		"""
		Go through all elements and dispatch the action.
		"""

		Error.assertHasAttr(element=element, attr="category")
		category = element.getAttr("category").value

		# Remove white spaces at the begining of the next ammendment
		if element.getAttrValue("stripLeft", False):
			result = result.rstrip()

		try:

			# Raw content
			if category == "content":
				Error.assertHasAttr(element=element, attr="content")
				result += element.getAttr("content").value

			# Substitution
			elif category == "substitution":
				result += self.visitSubstitution(element=element)

			# End statement
			elif category == "end":
				pass # nothing to do

			# For loop block
			elif category == "for":
				result += self.visitForBlock(element=element)

			# If block
			elif category == "if":
				Error.assertHasAttr(element=element, attr="condition")
				conditionStr = element.getAttr("condition").value
				result += self.visitIfBlock(element=element, conditionStr=conditionStr)

			# Else block
			elif category == "else":
				if self.followElse:
					conditionStr = element.getAttrValue("condition", "True")
					result += self.visitIfBlock(element=element, conditionStr=conditionStr)

			else:
				raise Exception("Unsupported category: '{}'.".format(category))

		# Propagate processed exception to the top layer
		except ExceptionParser as e:
			raise e

		except Exception as e:
			Error.handleFromElement(element=element, message=str(e))

		# hello    {{- hello -}} me   <- next
		# {for -} in {} <- each loop
		# {for} dsd
		# {- end}   <- 

		# Remove white spaces at the begining of the next ammendment
		#if element.getAttr("stripRight").value:
		#	result = result.rstrip()

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

		print("%%%%%%")
		print(sequence)

		result = VisitorTemplate(substitutions).visit(sequence)

		return result
