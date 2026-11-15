import pathlib
import typing

from bzd.parser.error import Error
from bzd.template.parser import Parser
from bzd.template.substitution import (
	SubstitutionsAccessor,
	SubstitutionsType,
	SubstitutionWrapper,
)
from bzd.template.visitor import ResultType, Visitor


class Template:
	def __init__(self, template: str, *args: typing.Any, **kwargs: typing.Any) -> None:
		self.parser = Parser(template)
		self.args = args
		self.kwargs = kwargs

	@staticmethod
	def fromPath(path: pathlib.Path, *args: typing.Any, **kwargs: typing.Any) -> "Template":
		template = Template("", *args, **kwargs)
		template.parser = Parser.fromPath(path)  # type: ignore
		return template

	def render(self, *args: SubstitutionsType) -> str:
		substitutions = SubstitutionsAccessor(*args)
		output, _ = self._render(substitutions=substitutions)
		return "".join(output)

	def _render(
		self, substitutions: SubstitutionsAccessor | SubstitutionWrapper
	) -> tuple[ResultType, SubstitutionWrapper]:
		sequence = self.parser.parse()
		# Get the last element of the sequence, if no element, use None
		last = None
		for last in sequence:
			pass
		if last and last.getAttrValue("category", None) in [
			"if",
			"else",
			"for",
			"macro",
		]:
			Error.handleFromElement(element=last, message="Unterminated control block.")

		visitor = Visitor(substitutions, *self.args, **self.kwargs)
		result = visitor.visit(sequence)

		return result, visitor.substitutions
