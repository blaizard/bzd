import re
import typing

from .fixed_table import FixedTable
from .parser import Parser


class ParserGcc(Parser):
	"""
    Gcc-generated map file parser.
    """

	def parse(self) -> bool:
		"""
        Parse the file.

        Returns:
                True in case of success, False otherwise.
        """

		with open(self.path, "r") as f:
			# Look for a line that ends with "memory map"
			if not self._goto(f, re.compile(r"^.*memory map$", re.IGNORECASE)):
				return False

			table: typing.Optional[FixedTable] = None
			for line in f:
				table = FixedTable.fromPattern(
				    line,
				    re.compile(
				        r"^(.*)\s(0x[0-9a-f]{8,})\s(\s+0x[0-9a-f]+)\s(.*)$",
				        re.IGNORECASE,
				    ),
				)
				if table:
					break

			if not table:
				return False

			# Go back to the beginning
			f.seek(0)
			self._goto(f, re.compile(r"^.*memory map$", re.IGNORECASE))

			sections = []
			units = []
			section = None
			isSection = False
			regexprHex = re.compile(r"^0x[0-9a-f]+$", re.IGNORECASE)

			for line in f:
				parts = table.parse(line)
				if parts.empty():
					continue

				# Identify the current section
				maybeSection = parts.get(0)

				if maybeSection:
					# Ignore *fill* sections
					if "*fill*" in maybeSection:
						continue

					# Sub sections are one space off, ignore them
					cellContent = parts.get(0, strip=False)
					assert cellContent
					if cellContent[0] != " ":
						# Some section are appended with some extra info, ignore it, such as:
						# - <section> memory region -> ram
						# - <section> memory region ->
						section = re.sub(r"memory\s+region\s+->.*$", "", maybeSection)
						isSection = True

				# If the data only contains the section, ignore as we have already handled it
				if parts.size() == 1:
					continue

				# Address and size must at least be defined to go further
				maybeAddress = parts.get(1)
				maybeSize = parts.get(2)
				if not maybeAddress or not maybeSize:
					continue
				if not regexprHex.match(maybeAddress) or not regexprHex.match(maybeSize):
					continue

				# Populate the section
				if section and isSection:
					sections.append({
					    "section": section,
					    "address": int(maybeAddress, 16),
					    "size": int(maybeSize, 16),
					})
				isSection = False

				# Populate the unit
				maybeUnit = parts.get(3)
				if maybeUnit:
					cellContent = parts.get(3, strip=False)
					assert cellContent
					if cellContent[1] != " ":
						m = re.match(r"^(?!LONG|load address)([^\(:]+).*$", maybeUnit)
						units.append({
						    "section": section,
						    "size": int(maybeSize, 16),
						    "units": m.group(1) if m else "undefined",
						})

			self.setParsedData(units=units, sections=sections, areUnitsPath=True)

		return True
