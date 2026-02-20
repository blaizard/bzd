import enum
import pathlib


class OutputFormat(enum.Enum):
	"""Output format supported."""

	JPEG = "jpeg"


class ConversionResult(enum.Enum):
	"""Result of a conversion."""

	completed = enum.auto()
	failed = enum.auto()
	# Use when the file is already considered 'good', no need to convert it.
	skipped = enum.auto()


class Converter:
	pass
