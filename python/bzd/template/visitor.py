import typing
import pathlib
import re

from bzd.parser.element import Element
from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error, ExceptionParser

# Needed by the include method
import bzd.template.template
from bzd.template.substitution import SubstitutionsAccessor, SubstitutionWrapper

ResultType = typing.List[str]


class Visitor(VisitorBase[ResultType, ResultType]):
	nestedKind = None

	def __init__(self,
		substitutions: typing.Union[SubstitutionsAccessor, SubstitutionWrapper],
		includeDirs: typing.Sequence[pathlib.Path] = [pathlib.Path(__file__).parent.parent.parent.parent],
		indent: bool = False) -> None:
		# Re-use directly substitution wrapper if provided.
		# This is needed by the include control block, as we want macros (for example) to be executed
		# with the current substition object and not a copy at a given time.
		self.substitutions = substitutions if isinstance(substitutions,
			SubstitutionWrapper) else SubstitutionWrapper(substitutions)
		self.includeDirs = includeDirs
		# Indent multiline substitution blocks to mmaatch the start of the block.
		self.indent = indent
		self.indentRegexpr = re.compile(r"(?:^|\n)([ \t]*)$")
		# Used to trigger the else condition, this works as any un-resolved if must be followed by the else (if any).
		self.followElse = False

	def visitBegin(self, result: ResultType) -> ResultType:
		return []

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
			# Note, the following conditions must remains eventhough they are handled by Substitution type
			# but only at the top level.
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

	def readValue(self, element: Element) -> typing.Any:
		"""
		Read a value from an element.
		"""

		Error.assertHasAttr(element=element, attr="value")
		Error.assertHasAttr(element=element, attr="type")
		valueType = element.getAttr("type").value
		value = element.getAttr("value").value
		if valueType == "name":
			return self.resolveName(value)
		elif valueType == "number":
			return float(value)
		elif valueType == "string":
			return value
		elif valueType == "true":
			return True
		elif valueType == "false":
			return False
		Error.handleFromElement(element=element, message="Unsupported value type.")

	def visitSubstitution(self, element: Element, result: ResultType) -> None:
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
				args.append(self.readValue(element=arg))
			value = self.resolveName(name, True, *args)

		# Process the pipes if any.
		pipes = element.getNestedSequence("pipe")
		if pipes:
			for pipe in pipes:
				Error.assertHasAttr(element=pipe, attr="name")
				value = self.resolveName(pipe.getAttr("name").value, True, value)

		# Save the output
		assert isinstance(value,
			(int, float, str, pathlib.Path)), "The resulting substitued value must be a number, a string or a path."
		self.appendSubstitution(element=element, result=result, string=str(value))

	def appendSubstitution(self, element: Element, result: ResultType, string: str) -> ResultType:

		# Apply indentation if enabled
		if self.indent and result:
			m = self.indentRegexpr.search(result[-1])
			if m:
				string = string.replace("\n", "\n" + m.group(1))

		result.append(string)
		return result

	def appendBlock(self, element: Element, result: ResultType, block: ResultType) -> ResultType:

		result += block
		return result

	def visitForBlock(self, element: Element) -> ResultType:
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

		block: ResultType = []

		# Loop through the elements
		iterable = self.resolveName(name=element.getAttr("iterable").value)
		if value2 is None:
			for value in iterable:
				self.substitutions.register(element=element, key=value1, value=value)
				block += self._visit(sequence=sequence)
				self.substitutions.unregister(value1)
		else:
			iterablePair = iterable.items() if isinstance(iterable, dict) else enumerate(iterable)
			for key, value in iterablePair:
				self.substitutions.register(element=element, key=value1, value=key)
				self.substitutions.register(element=element, key=value2, value=value)
				block += self._visit(sequence=sequence)
				self.substitutions.unregister(value2)
				self.substitutions.unregister(value1)

		return block

	def visitIfBlock(self, element: Element, conditionStr: str) -> ResultType:
		"""
		Handle if block.
		"""

		sequence = element.getNestedSequenceAssert(kind="nested")

		condition = self.evaluateCondition(conditionStr=conditionStr)
		self.followElse = not condition
		if condition:
			return self._visit(sequence=sequence)

		return []

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

		# Sanity check
		Error.assertTrue(element=element,
			condition=len(argList) == len(args),
			message="Wrong number of argument(s), expected {}: {}".format(len(argList), ", ".join(argList)))

		for i, name in enumerate(argList):
			self.substitutions.register(element=element, key=name, value=args[i])

		# Process the template
		result = self._visit(sequence=sequence)

		for name in argList:
			self.substitutions.unregister(name)

		return "".join(result)

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

	def visitInclude(self, element: Element) -> ResultType:
		"""
		Handle include.
		"""

		includePathStr = self.readValue(element=element)
		Error.assertTrue(element=element,
			condition=isinstance(includePathStr, str),
			message="The include path must resolve into a string, instead: '{}'.".format(includePathStr))
		path = pathlib.Path(includePathStr)
		paths = [(base / path) for base in self.includeDirs if (base / path).is_file()]
		Error.assertTrue(element=element,
			condition=len(paths) > 0,
			message="No valid file '{}' within {}".format(includePathStr,
			str([f.as_posix() for f in self.includeDirs])))

		template = bzd.template.template.Template(template=paths[0].read_text(),
			includeDirs=self.includeDirs,
			indent=self.indent)
		result, substitutions = template._render(substitutions=self.substitutions)

		# Update the current substitution object
		self.substitutions.update(substitutions)

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

	def visitElement(self, element: Element, result: ResultType) -> ResultType:
		"""
		Go through all elements and dispatch the action.
		"""

		Error.assertHasAttr(element=element, attr="category")
		category = element.getAttr("category").value

		try:

			# Raw content
			if category == "content":
				Error.assertHasAttr(element=element, attr="content")
				string = element.getAttr("content").value
				result.append(string)

			# Substitution
			elif category == "substitution":
				self.visitSubstitution(element=element, result=result)

			# Comments
			elif category == "comment":
				pass

			# End statement
			elif category == "end":
				# Reset the else flag here to make sure nested if without else do not trigger.
				self.followElse = False

			# For loop block
			elif category == "for":
				block = self.visitForBlock(element=element)
				self.appendBlock(element=element, result=result, block=block)

			# If block
			elif category == "if":
				Error.assertHasAttr(element=element, attr="condition")
				conditionStr = element.getAttr("condition").value
				block = self.visitIfBlock(element=element, conditionStr=conditionStr)
				self.appendBlock(element=element, result=result, block=block)

			# Else block
			elif category == "else":
				block = []
				if self.followElse:
					conditionStr = element.getAttrValue("condition", "True")  # type: ignore
					block = self.visitIfBlock(element=element, conditionStr=conditionStr)
				self.appendBlock(element=element, result=result, block=block)

			# Macro block
			elif category == "macro":
				self.visitMacro(element=element)

			elif category == "include":
				block = self.visitInclude(element=element)
				self.appendBlock(element=element, result=result, block=block)

			else:
				raise Exception("Unsupported category: '{}'.".format(category))

		# Propagate processed exception to the top layer
		except ExceptionParser as e:
			raise e

		except Exception as e:
			Error.handleFromElement(element=element, message=str(e))

		return result
