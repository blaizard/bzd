import re
import typing

class _Row:
	def __init__(self, table, parts):
		self.table = table
		self.parts = parts

	def get(self, identifier, strip = True) -> typing.Optional[str]:
		"""
		Access a single element from the row.
		"""
		if isinstance(identifier, int):
			assert identifier >= 0, "Index {} out of bounds.".format(identifier)
			assert identifier < self.table.nbColumns(), "Index {} out of bounds.".format(identifier)
			if identifier < self.size():
				data = self.parts[identifier]
				return data.strip() if strip else data
			return None

		if isinstance(identifier, str):
			assert hasattr(self.table, "nameToIndex"), "String identifiers are not supported by this table."
			assert identifier in self.table.nameToIndex, "The string '{}' is not a known identifier.".format(identifier)
			return self.get(self.table.nameToIndex[identifier], strip)

		raise AttributeError("Unsupported argument type {}.".format(type(identifier)))

	def size(self):
		"""
		Get the number of elements from the row.
		"""
		return len(self.parts)

	def empty(self):
		"""
		Tells if a row is empty or not.
		"""
		return self.size() == 0

class FixedTable:

	def __init__(self, separations, nameToIndex = None):
		self.separations = separations
		self.nameToIndex = nameToIndex if nameToIndex else {}

	@staticmethod
	def fromHeader(header, schema):
		separations = [0]
		nameToIndex = {}

		start = 0
		for name in header.split():
			result = header.find(name, start)
			assert result > -1, "Couldn't find {}".format(result)
			curSchema = schema.get(name, {})

			if curSchema.get("align", "left") == "right":
				end = result + len(name) + 1
			else:
				end = result + len(name)
				nbBlanks = re.search(r'\S', header[end:])
				end += nbBlanks.start() if nbBlanks else 0

			assert name not in nameToIndex, "Cell name already discovered: {}".format(name)
			nameToIndex[name] = len(separations) - 1
			separations.append(end - 1)
			start = end

		return FixedTable(separations, nameToIndex)

	@staticmethod
	def fromPattern(line, regexpr):
		m = regexpr.match(line)
		if not m:
			return None
		separations = [0]
		for i in range(1, m.lastindex + 1):
			separations.append(m.end(i))
		print(line)
		print(separations)
		return FixedTable(separations)

	def nbColumns(self):
		return len(self.separations) - 1

	def parse(self, line):

		# Split the data into cells 
		parts = [line[i:j] for i,j in zip(self.separations, self.separations[1:]+[None])]

		# Handle cell content overflow
		carry = ""
		for i in range(len(parts)-1, -1, -1):
			if parts[i]:
				# Wrap content that is not the first cell, and that has content
				# on the first position or is further than the number of columns.
				if i and (parts[i][0] != ' ' or i >= self.nbColumns()):
					carry = parts[i] + carry
					parts[i] = ""
				else:
					parts[i] += carry
					carry = ""

		# Remove the right most elements if empty
		while len(parts) and parts[-1] == "":
			parts.pop()

		return _Row(self, parts)

"""

                0x0000000000000000       0x40

[0, 15, 33, 44, 45]



                0x0000000000000000    0x3347f

||0x0000000000000000    0x3347f|||
[0, 15, 33, 44, 45]
"""