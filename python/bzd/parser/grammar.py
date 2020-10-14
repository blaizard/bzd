from typing import Optional, Dict, List, Pattern, Union, Type, TYPE_CHECKING
from bzd.parser.fragments import Fragment

Grammar = List["GrammarItem"]


class GrammarItem:

	def __init__(self,
		regexpr: str,
		fragment: Union[Type[Fragment], str] = Fragment,
		grammar: Optional[Grammar] = None) -> None:
		self.regexpr = regexpr
		if isinstance(fragment, str):
			class Temp(Fragment):
				default = {str(fragment): ""}

			self.fragment = Temp
		else:
			self.fragment = fragment # type: ignore
		self.grammar = grammar

	def __repr__(self) -> str:
		return str(self.regexpr)


GrammarItemSpaces = GrammarItem(r"[\s]+")
