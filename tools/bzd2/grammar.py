from typing import Optional, Sequence, Pattern, TYPE_CHECKING

if TYPE_CHECKING:
	from tools.bzd2.fragments import Fragment

Grammar = Sequence["GrammarItem"]


class GrammarItem:

	def __init__(
			self,
			regexpr: Pattern[str],
			fragment: Optional["Fragment"] = None,
			grammar: Optional[Grammar] = None) -> None:
		self.regexpr = regexpr
		self.fragment = fragment
		self.grammar = grammar

	def __repr__(self) -> str:
		return str(self.regexpr)
