import re
import pathlib
import typing

SectionType = typing.Dict[str, int]
UnitsType = typing.Dict[str, typing.Dict[str, int]]
Fragment = typing.Sequence[typing.Dict[str, typing.Any]]


class Parser:
	"""
	Parser base class.
	"""

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path
		self.setParsedData()

	def setParsedData(self, sections: typing.Optional[Fragment] = None,
		units: typing.Optional[Fragment] = None) -> None:
		"""
		Set the raw data from the specialized parser.

		Args:
			sections: Section fragments.
			units: Units fragments.
		"""

		self.sections: SectionType = {}
		self.units: UnitsType = {}
		if sections:
			self.sections = self._aggregateSections(sections)
		if units:
			self.units = self._aggregateUnits(units)

	def parse(self) -> bool:
		"""
		Parse the file.

		Returns:
			True in case of success, False otherwise.
		"""

		return False

	@staticmethod
	def _goto(f: typing.TextIO, regexpr: typing.Pattern[str]) -> bool:
		"""
		Go to a line that matches the regular expression, if not found, return False.

		Args:
			f: The IO instance.
			regexpr: Regular expression to be matched.

		Returns:
			True if found, False otherwise.
		"""
		for line in f:
			if regexpr.match(line):
				return True
		return False

	@staticmethod
	def _aggregateSections(sections: Fragment) -> SectionType:
		"""
		Aggregates and cleanup section fragments.

		Args:
			sections: Section fragments.

		Returns:
			The aggregated sections.
		"""

		aggregatedSections: SectionType = {}
		# Combine sections
		for item in sections:
			if item["section"] not in aggregatedSections:
				aggregatedSections[item["section"]] = 0
			aggregatedSections[item["section"]] += item["size"]
		# Remove empty sections
		return {section: size for section, size in aggregatedSections.items() if size > 0}

	@staticmethod
	def _aggregateUnits(units: Fragment) -> UnitsType:
		"""
		Aggregates and cleanup unit fragments.

		Args:
			units: Unit fragments.

		Returns:
			The aggregated units.
		"""

		aggregatedUnits: UnitsType = {}
		for entry in units:
			if entry["units"] not in aggregatedUnits:
				aggregatedUnits[entry["units"]] = {}
			data = aggregatedUnits[entry["units"]]
			if entry["section"] not in data:
				data[entry["section"]] = 0
			data[entry["section"]] += entry["size"]
		# Remove empty sections
		for unit, sections in aggregatedUnits.items():
			aggregatedUnits[unit] = {section: size for section, size in sections.items() if size > 0}
		return aggregatedUnits

	def getBySections(self) -> SectionType:
		"""
		Accessor of the sections.

		Returns:
			The sections.
		"""

		return self.sections

	def getByUnits(self) -> UnitsType:
		"""
		Accessor of the units.

		Returns:
			The units.
		"""

		return self.units

	@staticmethod
	def _patternToRegexpr(pattern: str) -> typing.Pattern[str]:
		"""
		Convert a pattern into a compiled regexpr.

		Args:
			pattern: The pattern string, where '*' are converted into r'.*'

		Returns:
			The compiled regular expression.
		"""

		regexpr = r''
		for fragment in pattern.split("*"):
			regexpr += re.escape(fragment) if fragment else r'.*'
		return re.compile(regexpr)

	def filter(self, pattern: str) -> None:
		"""
		Filter out a specific pattern.

		Args:
			pattern: The pattern to filter out.
		"""

		regexpr = self._patternToRegexpr(pattern)
		# Filter sections
		self.sections = {section: data for section, data in self.sections.items() if not regexpr.fullmatch(section)}
		# Filter units
		for unit, sections in self.units.items():
			self.units[unit] = {section: data for section, data in sections.items() if not regexpr.fullmatch(section)}
