from typing import Optional, Sequence, Pattern, Union, TYPE_CHECKING
from bzd.parser.fragments import Fragment

Grammar = Sequence["GrammarItem"]


class GrammarItem:

	def __init__(self,
		regexpr: Pattern[str],
		fragment: Union[Fragment, str] = Fragment,
		grammar: Optional[Grammar] = None) -> None:
		self.regexpr = regexpr
		if isinstance(fragment, str):

			class Temp(Fragment):
				default = {fragment: ""}

			self.fragment = Temp
		else:
			self.fragment = fragment
		self.grammar = grammar

	def __repr__(self) -> str:
		return str(self.regexpr)


GrammarItemSpaces = GrammarItem(r"[\s]+")
