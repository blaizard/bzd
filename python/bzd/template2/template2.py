import typing
import pathlib
import re

from bzd.parser.element import Element
from bzd.parser.visitor import Visitor
from bzd.parser.error import Error, ExceptionParser

from bzd.template2.parser import Parser

SubstitutionsType = typing.MutableMapping[str, typing.Any]
ResultType = typing.List[str]

_defaultValue: typing.Any = {}


class SubstitutionWrapper:

	def __init__(self, substitutions: SubstitutionsType) -> None:
		self.substitutions = substitutions
		self.ext: typing.Dict[str, typing.Any] = {}

	def register(self, element: Element, key: str, value: typing.Any) -> None:
		Error.assertTrue(element=element,
			condition=(key not in self),
			message="Name conflict, '{}' already exists in the substitution map.".format(key))
		self.ext[key] = value

	def __getitem__(self, key: str) -> typing.Any:
		if hasattr(self.substitutions, key):
			return getattr(self.substitutions, key)
		elif key in self.substitutions:
			return self.substitutions[key]
		return self.ext[key]

	def __delitem__(self, key: str) -> None:
		del self.ext[key]

	def __contains__(self, key: str) -> bool:
		return hasattr(self.substitutions, key) or key in self.substitutions or key in self.ext


class VisitorTemplate(Visitor[ResultType, str]):
	nestedKind = None

	def __init__(self, substitutions: SubstitutionsType) -> None:
		self.substitutions = SubstitutionWrapper(substitutions)
		# Used to trigger the else condition, this works as any un-resolved if must be followed by the else (if any).
		self.followElse = False
		# Used to strip the next element after a this flag is set.
		# This is used by substitutions.
		self.stripLeftNextResult: typing.Optional[str] = None

	def visitBegin(self, result: ResultType) -> ResultType:
		return []

	def visitFinal(self, result: ResultType) -> str:
		return "".join(result)

	def resolveName(self, name: str, isCallable: bool = False, *args: typing.Any) -> typing.Any:
		"""
		Resolve a name from the current substitution list.
		Names are splited at the '.' character and each symbol is matched with the current
		substitution list.
		If this is a callable, it will be resolved.
		"""
		value: typing.Any = self.substitutions

		for symbol in name.split("."):

			# If this is a callable, process its value
			if callable(value):
				value = value()

			# Look for the corresponding nested value
			if hasattr(value, symbol):
				value = getattr(value, symbol)
			elif symbol in value:
				value = value[symbol]
			else:
				raise Exception("Substitution value for '{}' does not exists.".format(name))

		if isCallable:
			assert callable(value), "'{}' must be callable.".format(name)
			value = value(*args)
		elif callable(value):
			value = value()

		return value

	def visitSubstitution(self, element: Element) -> ResultType:
		"""
		Handle substitution.
		"""
		Error.assertHasAttr(element=element, attr="name")
		name = element.getAttr("name").value

		# Check if callable and if there are arguments
		arguments = element.getNestedSequence("argument")
		if arguments is None:
			value = self.resolveName(name=name)
		else:
			# Resolve all arguments
			args = []
			for arg in arguments:
				Error.assertHasAttr(element=arg, attr="name")
				args.append(self.resolveName(arg.getAttr("name").value))
			value = self.resolveName(name, True, *args)

		# Process the pipes if any.
		pipes = element.getNestedSequence("pipe")
		if pipes:
			for pipe in pipes:
				Error.assertHasAttr(element=pipe, attr="name")
				value = self.resolveName(pipe.getAttr("name").value, True, value)

		# Save the output
		assert isinstance(value, (int, float, str)), "The resulting substitued value must be a number or a string."
		return [str(value)]

	def processNestedResult(self, element: Element, result: ResultType) -> ResultType:
		"""
		Format the nested result before returning it.
		"""
		if result:
			if element.getAttrValue("stripRight"):
				result[0] = result[0].lstrip()
			if element.getAttrValue("nestedStripRight"):
				result[-1] = result[-1].rstrip()
		return result

	def visitForBlock(self, element: Element, result: ResultType) -> None:
		"""
		Handle for loop block.
		"""

		Error.assertHasAttr(element=element, attr="value1")
		Error.assertHasAttr(element=element, attr="iterable")
		Error.assertHasSequence(element=element, sequence="nested")

		value1 = element.getAttr("value1").value
		value2 = element.getAttrValue("value2")

		sequence = element.getNestedSequence(kind="nested")
		assert sequence

		# Loop through the elements
		iterable = self.resolveName(name=element.getAttr("iterable").value)
		if value2 is None:
			for value in iterable:
				self.substitutions.register(element=element, key=value1, value=value)
				output = self._visit(sequence=sequence)
				result += self.processNestedResult(element=element, result=output)
				del self.substitutions[value1]
		else:
			iterablePair = iterable.items() if isinstance(iterable, dict) else enumerate(iterable)
			for key, value in iterablePair:
				self.substitutions.register(element=element, key=value1, value=key)
				self.substitutions.register(element=element, key=value2, value=value)
				output = self._visit(sequence=sequence)
				result += self.processNestedResult(element=element, result=output)
				del self.substitutions[value2]
				del self.substitutions[value1]

	def visitIfBlock(self, element: Element, result: ResultType, conditionStr: str) -> None:
		"""
		Handle if block.
		"""

		Error.assertHasSequence(element=element, sequence="nested")
		sequence = element.getNestedSequence(kind="nested")
		assert sequence

		condition = self.evaluateCondition(conditionStr=conditionStr)
		self.followElse = not condition
		if condition:
			output = self._visit(sequence=sequence)
			result += self.processNestedResult(element=element, result=output)

	def processMacro(self, element: Element, *args: typing.Any) -> str:
		"""
		Process a macro already defined.
		"""

		sequence = element.getNestedSequence("nested")
		assert sequence

		# Build the argument list
		argList = []
		argument = element.getNestedSequence("argument")
		assert argument is not None
		for arg in argument:
			Error.assertHasAttr(element=arg, attr="name")
			argList.append(arg.getAttr("name").value)

		for i, name in enumerate(argList):
			self.substitutions.register(element=element, key=name, value=args[i])

		# Process the template
		output = self._visit(sequence=sequence)
		output = self.processNestedResult(element=element, result=output)

		for name in argList:
			del self.substitutions[name]

		return "".join(output)

	def visitMacro(self, element: Element) -> None:
		"""
		Handle macro definition block.
		"""
		Error.assertHasSequence(element=element, sequence="argument")
		Error.assertHasSequence(element=element, sequence="nested")
		Error.assertHasAttr(element=element, attr="name")

		name = element.getAttr("name").value
		Error.assertTrue(element=element,
			attr="name",
			condition=(name not in self.substitutions),
			message="Name conflict with macro and an already existing name: '{}'.".format(name))

		# Register the macro
		self.substitutions.register(element=element, key=name, value=lambda *args: self.processMacro(element, *args))

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

	def visitElement(self, element: Element, result: ResultType) -> ResultType:
		"""
		Go through all elements and dispatch the action.
		"""

		Error.assertHasAttr(element=element, attr="category")
		category = element.getAttr("category").value

		# Remove white spaces at the begining of the next ammendment
		if result and element.getAttrValue("stripLeft"):
			result[-1] = result[-1].rstrip()

		try:

			output = []

			# Raw content
			if category == "content":
				Error.assertHasAttr(element=element, attr="content")
				output = [element.getAttr("content").value]

			# Substitution
			elif category == "substitution":
				output = self.visitSubstitution(element=element)

			# Comments
			elif category == "comment":
				pass  # nothing to do

			# End statement
			elif category == "end":
				pass  # nothing to do

			# For loop block
			elif category == "for":
				self.visitForBlock(element=element, result=output)

			# If block
			elif category == "if":
				Error.assertHasAttr(element=element, attr="condition")
				conditionStr = element.getAttr("condition").value
				self.visitIfBlock(element=element, result=output, conditionStr=conditionStr)

			# Else block
			elif category == "else":
				if self.followElse:
					conditionStr = element.getAttrValue("condition", "True")  # type: ignore
					self.visitIfBlock(element=element, result=output, conditionStr=conditionStr)

			# Macro block
			elif category == "macro":
				self.visitMacro(element=element)

			else:
				raise Exception("Unsupported category: '{}'.".format(category))

			if output and self.stripLeftNextResult:
				output[0] = output[0].lstrip()
			self.stripLeftNextResult = element.getAttrValue("stripRight")

			# Update the final result
			result += output

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
		result = VisitorTemplate(substitutions).visit(sequence)

		return result
