import re
import typing

PartsType = typing.List[str]


class _Row:
	"""
    Table row accessor class.
    """

	def __init__(self, table: "FixedTable", parts: PartsType) -> None:
		self.table = table
		self.parts = parts

	def get(self, identifier: typing.Union[int, str], strip: bool = True) -> typing.Optional[str]:
		"""
        Access a single element from the row.

        Args:
                identifier: The cell position identifier within the row.
                strip: Whether stripped output is needed or not.

        Returns:
                The element, or None if no element is present.
        """
		assert self.table.isValidIdentifier(identifier), "Invalid identifier: {}".format(identifier)

		if isinstance(identifier, int):
			if identifier < self.size():
				data = self.parts[identifier]
				return data.strip() if strip else data
			return None

		if isinstance(identifier, str):
			return self.get(self.table.nameToIndex[identifier], strip)

	def size(self) -> int:
		"""
        Get the number of elements from the row.

        Returns:
                The size of the row.
        """
		return len(self.parts)

	def empty(self) -> bool:
		"""
        Tells if a row is empty or not.

        Returns:
                True if there is no element in the row, False otherwise.
        """
		return self.size() == 0


class FixedTable:
	"""
    A fixed table is a table which columns have a fixed width across the rows.
    """

	def __init__(
	    self,
	    separations: typing.List[int],
	    nameToIndex: typing.Optional[typing.Dict[str, int]] = None,
	) -> None:
		self.separations = separations
		self.nameToIndex = nameToIndex if nameToIndex else {}

	def isValidIdentifier(self, identifier: typing.Union[int, str]) -> bool:
		"""
        Tells if a specific identifier is valid.

        Args:
                identifier: The identifier to be tested.

        Returns:
                True if it is a valid id, False otherwise.
        """
		if isinstance(identifier, int):
			return identifier >= 0 and identifier < self.nbColumns()

		if isinstance(identifier, str):
			return identifier in self.nameToIndex

		return False

	@staticmethod
	def fromHeader(header: str, schema: typing.Dict[str, typing.Any]) -> typing.Optional["FixedTable"]:
		"""
        Make a FixedTable instance from the header line.

        Args:
                header: The header string line.
                schema: A schema describing the columns.

        Returns:
                A FixedTable instance in case of success, None otherwise.
        """

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
				nbBlanks = re.search(r"\S", header[end:])
				end += nbBlanks.start() if nbBlanks else 0

			assert name not in nameToIndex, "Cell name already discovered: {}".format(name)
			nameToIndex[name] = len(separations) - 1
			separations.append(end - 1)
			start = end

		return FixedTable(separations, nameToIndex)

	@staticmethod
	def fromPattern(line: str, regexpr: typing.Pattern[str]) -> typing.Optional["FixedTable"]:
		"""
        Make a FixedTable instance from a row and a regular expression.

        Args:
                line: The row string line.
                regexpr: The regular expression where each capture element corresponds to a cell.

        Returns:
                A FixedTable instance in case of success, None otherwise.
        """

		m = regexpr.match(line)
		if not m:
			return None
		separations = [0]
		assert m.lastindex
		for i in range(1, m.lastindex + 1):
			separations.append(m.end(i))
		return FixedTable(separations)

	def nbColumns(self) -> int:
		"""
        Return the number of columns of this table.

        Returns:
                The number of columns.
        """

		return len(self.separations) - 1

	def parse(self, line: str) -> _Row:
		"""
        Parse a new string line.

        Args:
                line: The row string line.

        Returns:
                A row accessor of the parsed line.
        """

		# Split the data into cells
		parts = [line[i:j] for i, j in zip(self.separations, self.separations[1:] + [len(line)])]

		# Handle cell content overflow
		carry = ""
		for i in range(len(parts) - 1, -1, -1):
			if parts[i]:
				# Wrap content that is not the first cell, and that has content
				# on the first position or is further than the number of columns.
				if i and (parts[i][0] != " " or i >= self.nbColumns()):
					carry = parts[i] + carry
					parts[i] = ""
				else:
					parts[i] += carry
					carry = ""

		# Remove the right most elements if empty
		while len(parts) and parts[-1] == "":
			parts.pop()

		return _Row(self, parts)
