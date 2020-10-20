import re
import typing
from pathlib import Path

SubstitutionType = typing.Dict[str, typing.Any]
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
		self.pattern = re.compile("\{([^\{]+)\}")
		self.patternIf = re.compile("^if\s+([^\{]+)$")
		self.patternFor = re.compile("^for\s+([^\s]+)\s+in\s+([^\s]+)$")
		self.patternForIndex = re.compile("^for\s+([^\s]+)\s*,\s*([^\s]+)\s+in\s+([^\s]+)$")
		self.patternSet = re.compile("^[^\s]+$")
		self.patternInclude = re.compile("^include\s+([^\s]+)\s*$")
		self.patternEnd = re.compile("^end$")
		self.patternWord = re.compile("[^\s]+")

	def _processInclude(self, content: str, args: SubstitutionType) -> str:
		template = Template(template=content, includeDirs=self.includeDirs)
		return template.process(args)

	def _getValue(self, args: SubstitutionType, key: str) -> typing.Any:

		for k in key.split("."):
			assert k in args, "Template value '{}' is not set.".format(key)
			args = args[k]() if callable(args[k]) else args[k]

		return args

	def _evalCondition(self, args: SubstitutionType, conditionStr: str) -> bool:

		def replaceValue(match: typing.Match[str]) -> str:
			if match.group() not in ["and", "or", "not", "(", ")"]:
				value = args
				for k in match.group().split("."):
					if k not in value:
						return match.group()
					value = value[k]
				# Replace the value
				if isinstance(value, bool):
					return str(value)
				elif isinstance(value, str):
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

	def process(self, args: SubstitutionType, removeEmptyLines: bool = True, recursive: bool = False) -> str:
		processedTemplate = self.template

		# Process the template
		nbIterations = 0
		while True:
			processedTemplate, isProcessed = self._process(processedTemplate, args)
			if not isProcessed or not recursive:
				break
			nbIterations += 1
			if nbIterations > 10:
				raise Exception("Too many iterations (>10)")

		processedTemplate = processedTemplate.replace("{{", "{")
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
					result, _ = self._process(template[index:len(template)], args)
					output += result
				ignoreOutput += 1
				continue

			# For loop
			matchFor = self.patternFor.match(operation)
			if matchFor:
				if not ignoreOutput:
					for value in self._getValue(args, matchFor.group(2)):
						args[matchFor.group(1)] = value
						result, _ = self._process(template[index:len(template)], args)
						output += result
						del args[matchFor.group(1)]

				ignoreOutput += 1
				continue

			# For loop with index
			matchForIndex = self.patternForIndex.match(operation)
			if matchForIndex:
				if not ignoreOutput:
					valueObject = self._getValue(args, matchForIndex.group(3))
					if isinstance(valueObject, list):
						iterator = enumerate(valueObject)
					else:
						iterator = valueObject.items()
					for key, value in iterator:
						args[matchForIndex.group(1)] = str(key)
						args[matchForIndex.group(2)] = value
						result, _ = self._process(template[index:len(template)], args)
						output += result
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
					output += self._processInclude(paths[0].read_text(), args)
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
				if not ignoreOutput:
					output += str(self._getValue(args, matchSet.group(0)))
				continue

			raise Exception("Template operation '{}' is not valid.".format(operation))

		output += template[index:len(template)]
		return output, isProcessed
