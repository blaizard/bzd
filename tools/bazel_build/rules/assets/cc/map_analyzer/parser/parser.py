import re
import os
import pathlib
import typing

SectionType = typing.Dict[str, int]
AggregatedSectionType = typing.Dict[str, typing.Dict[str, int]]
UnitsType = typing.Dict[str, typing.Dict[str, int]]
AggregatedUnitsType = typing.Dict[str, typing.Dict[str, int]]
Fragment = typing.Sequence[typing.Dict[str, typing.Any]]


class Parser:
	"""
	Parser base class.
	"""

	unsetAggregrate = "unknown"

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path
		self.mapping: typing.Dict[str, str] = {}
		self.setParsedData()

	def setParsedData(self,
		sections: typing.Optional[Fragment] = None,
		units: typing.Optional[Fragment] = None,
		areUnitsPath: bool = False) -> None:
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
			self.mapping = {section: Parser.unsetAggregrate for section in self.sections}
		if units:
			self.units = self._aggregateUnits(units, areUnitsPath)

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
	def _aggregateUnits(units: Fragment, areUnitsPath: bool) -> UnitsType:
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
		# Normalize units if it represents path
		if areUnitsPath:
			return {os.path.normpath(key): value for key, value in aggregatedUnits.items()}
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

	def getByAggregatedSections(self) -> AggregatedSectionType:
		"""
		Accessor of the aggregated sections.

		Returns:
			The sections.
		"""
		sections: AggregatedSectionType = {}
		for section, size in self.sections.items():
			aggregate = self.mapping.get(section, Parser.unsetAggregrate)
			if aggregate not in sections:
				sections[aggregate] = {}
			sections[aggregate].update({section: size})

		return sections

	def getByAggregatedUnits(self) -> AggregatedUnitsType:
		"""
		Accessor of the aggregated units.

		Returns:
			The units.
		"""

		units: AggregatedUnitsType = {}
		for name, sections in self.units.items():
			units[name] = {}
			for section, size in sections.items():
				aggregate = self.mapping.get(section, Parser.unsetAggregrate)
				if aggregate not in sections:
					units[name][aggregate] = 0
				units[name][aggregate] += size

		return units

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

	def addAggregation(self, name: str, patternList: typing.Sequence[str]) -> None:
		"""
		Add aggregation mapping.

		Args:
			name: Aggregate categroy name.
			patternList: List of pattern to map with the name provided.
		"""

		regexprList = [self._patternToRegexpr(pattern) for pattern in patternList]
		for section, aggregate in self.mapping.items():
			if aggregate == Parser.unsetAggregrate:
				if any(regexpr.fullmatch(section) for regexpr in regexprList):
					self.mapping[section] = name
