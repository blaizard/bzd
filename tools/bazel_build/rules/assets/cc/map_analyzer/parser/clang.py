import re
import typing

from .fixed_table import FixedTable
from .parser import Parser


class ParserClang(Parser):
	"""
	Clang-generated map file parser. 
	"""

	def parse(self) -> bool:
		"""
		Parse the file.

		Returns:
			True in case of success, False otherwise.
		"""

		with open(self.path, "r") as f:

			# First line corresponds to the formating.
			table = FixedTable.fromHeader(header=f.readline().lower(),
				schema={
				"vma": {
				"align": "right"
				},
				"lma": {
				"align": "right"
				},
				"size": {
				"align": "right"
				},
				"align": {
				"align": "right"
				},
				"out": {
				"align": "left"
				},
				"in": {
				"align": "left"
				},
				"symbol": {
				"align": "left"
				},
				})

			if not table:
				return False

			if not table.isValidIdentifier("size") or not table.isValidIdentifier("out") or not table.isValidIdentifier(
				"in"):
				return False

			section = None
			units = []
			sections = []
			# Parse the content of the table
			for line in f:
				parts = table.parse(line)

				maybeSize = parts.get("size")
				if not maybeSize:
					continue

				# Extract important data
				cellOut = parts.get("out")
				if cellOut:
					section = cellOut
					sections.append({"section": section, "size": int(maybeSize, 16)})

				cellIn = parts.get("in")
				if cellIn:
					m = re.match(r'^([^\(:]+).*$', cellIn)
					if m:
						units.append({"section": section, "size": int(maybeSize, 16), "units": m.group(1)})

			self.setParsedData(units=units, sections=sections)
		return True
