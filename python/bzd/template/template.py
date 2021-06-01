import re
import typing
from pathlib import Path

# Can be anything, a dictionary, an object with properties...
SubstitutionType = typing.Any
Iterator = typing.Iterable[typing.Any]

_defaultValue: typing.Any = {}
"""
Process a template with specific values.
All block {<action>} must end with {end}, actions can be:
- {if <condition>}: Process the following block only if the condition evaluates to True.
- {for <variable> in <identifier>} or {for <key>, <value> in <identifier>}: Run the block as many times as there are entries.
- {include <path>}: Include a template while processing the template.
All strings "{{" are replace with "{".
"""


class Template:

	def __init__(
			self,
			template: str,
			includeDirs: typing.Sequence[Path] = [Path(__file__).parent.parent.parent.parent]) -> None:
		self.template = template
		self.includeDirs = includeDirs

		# Pre-process regexpr
		self.pattern = re.compile("{([^}{]*)}")
		self.patternIf = re.compile("^if\s+([^\{]+)$")
		self.patternFor = re.compile("^for\s+([^\s]+)\s+in\s+([^\s]+)$")
		self.patternForIndex = re.compile("^for\s+([^\s]+)\s*,\s*([^\s]+)\s+in\s+([^\s]+)$")
		self.patternSet = re.compile("^([^\s]+(\s*|\*)?)+$")
		self.patternInclude = re.compile("^include\s+([^\s]+)\s*$")
		self.patternEnd = re.compile("^end$")
		self.patternWord = re.compile("[^\s]+")

	def _getValue(self, args: SubstitutionType, key: str, param: typing.Any = _defaultValue) -> typing.Any:
		"""
		Return a value from the argument list given a key.
		"""

		for k in key.split("."):

			if hasattr(args, k):
				args = getattr(args, k)
			elif k in args:
				args = args[k]
			else:
				raise Exception("Template value '{}' is not set.".format(key))

			if param is not _defaultValue:
				assert callable(args), "'{}' must be callable.".format(key)
				args = args(param)
			elif callable(args):
				args = args()

		return args

	def _isNumber(self, potentialNumber: str) -> bool:
		"""
		Check if a string is convertible into a number.
		"""
		try:
			float(potentialNumber)
		except:
			return False
		return True

	def _evalCondition(self, args: SubstitutionType, conditionStr: str) -> bool:

		def replaceValue(match: typing.Match[str]) -> str:
			if match.group() not in ["and", "or", "not", "(", ")"]:

				# Try to read the value
				try:
					value = self._getValue(args=args, key=match.group())
				except:
					return match.group()

				# Replace the value
				if isinstance(value, bool):
					return str(value)
				elif isinstance(value, str):
					if self._isNumber(value):
						return value
					return "\"{}\"".format(value)
				elif isinstance(value, list):
					return str(bool(value))
				elif isinstance(value, dict):
					return str(bool(value))
				raise Exception("Unsupported type for value '{}'.".format(match.group()))
			return match.group()

		conditionStr = re.sub(self.patternWord, replaceValue, conditionStr)
		try:
			condition = eval(conditionStr)
		except:
			raise Exception("Cannot evaluate condition '{}'.".format(conditionStr))
		return bool(condition)

	def prepareTemplate(self, template: str) -> str:
		template = template.replace("{{", "\x01")
		return template

	def process(self, args: SubstitutionType, removeEmptyLines: bool = True, recursive: bool = False) -> str:
		processedTemplate = self.prepareTemplate(self.template)

		# Process the template
		nbIterations = 0
		while True:
			processedTemplate, isProcessed = self._process(processedTemplate, args)
			if not isProcessed or not recursive:
				break
			nbIterations += 1
			if nbIterations > 10:
				raise Exception("Too many iterations (>10)")

		processedTemplate = processedTemplate.replace("\x01", "{")
		processedTemplate = processedTemplate.replace("}}", "}")
		if removeEmptyLines:
			processedTemplate = "\n".join([line for line in processedTemplate.split("\n") if line.strip() != ""])
		return processedTemplate

	def _process(self, template: str, args: SubstitutionType) -> typing.Tuple[str, bool]:

		index = 0
		output = ""
		ignoreOutput = 0
		isProcessed = False

		for match in self.pattern.finditer(template):

			# Tells that at least something has been done
			isProcessed = True

			# Update the output string and the new index
			if not ignoreOutput:
				output += template[index:match.start()]
			index = match.end()

			# Identify the operation
			operation = match.group(1).strip()

			# If block
			matchIf = self.patternIf.match(operation)
			if matchIf:
				if (not ignoreOutput) and self._evalCondition(args, matchIf.group(1)):
					output += self._process(template[index:len(template)], args)[0]
				ignoreOutput += 1
				continue

			# For loop
			matchFor = self.patternFor.match(operation)
			if matchFor:
				if not ignoreOutput:
					for value in self._getValue(args, matchFor.group(2)):
						args[matchFor.group(1)] = value
						output += self._process(template[index:len(template)], args)[0]
						del args[matchFor.group(1)]

				ignoreOutput += 1
				continue

			# For loop with index
			matchForIndex = self.patternForIndex.match(operation)
			if matchForIndex:
				if not ignoreOutput:
					valueObject = self._getValue(args, matchForIndex.group(3))

					iterator: Iterator = valueObject.items() if isinstance(valueObject,
						dict) else enumerate(valueObject)

					for key, value in iterator:
						args[matchForIndex.group(1)] = str(key)
						args[matchForIndex.group(2)] = value
						output += self._process(template[index:len(template)], args)[0]
						del args[matchForIndex.group(2)]
						del args[matchForIndex.group(1)]

				ignoreOutput += 1
				continue

			# Include pattern
			matchInclude = self.patternInclude.match(operation)
			if matchInclude:
				if not ignoreOutput:
					path = Path(matchInclude.group(1))
					# Look for the template
					paths = [(base / path) for base in self.includeDirs if (base / path).is_file()]
					assert len(paths) > 0, "Cannot find template '{}' from any paths: {}".format(
						path, ", ".join([base.as_posix() for base in self.includeDirs]))
					includeTemplate = self.prepareTemplate(paths[0].read_text())
					output += self._process(template=includeTemplate, args=args)[0]
				continue

			# End pattern
			matchEnd = self.patternEnd.match(operation)
			if matchEnd:
				if not ignoreOutput:
					return output, True
				ignoreOutput -= 1
				continue

			# Set value
			matchSet = self.patternSet.match(operation)
			if matchSet:
				matchList = [s.strip() for s in matchSet.group(0).split("|")]
				assert len(matchList) > 0, "No valid item found in '{}'".format(matchSet.group(0))
				if not ignoreOutput:
					value = self._getValue(args, matchList[0])
					for item in matchList[1:]:
						value = self._getValue(args, item, value)
					output += str(value)
				continue

			raise Exception("Template operation '{}' is not valid.".format(operation))

		output += template[index:len(template)]
		return output, isProcessed
