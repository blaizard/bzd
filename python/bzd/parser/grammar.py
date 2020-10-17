from typing import Any, Optional, Dict, List, Pattern, Union, Type, TYPE_CHECKING
from bzd.parser.fragments import Fragment

Grammar = List[Any]


class GrammarItem:

	def __init__(self,
		regexpr: str,
		fragment: Union[Type[Fragment], Dict[str, str]] = Fragment,
		grammar: Optional[Grammar] = None) -> None:

		self.regexpr = regexpr
		if isinstance(fragment, dict):

			class SimpleFragment(Fragment):
				default = fragment.copy() # type: ignore

			self.fragment = SimpleFragment

		elif isinstance(fragment, type(Fragment)):
			self.fragment = fragment  # type: ignore

		else:
			raise Exception("Unsupported type for GrammarItem::fragment argument: {}".format(type(fragment)))

		self.grammar = grammar

	def __repr__(self) -> str:
		return str(self.regexpr)


GrammarItemSpaces = GrammarItem(r"[\s]+")
