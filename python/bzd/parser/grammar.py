import re
import typing
from functools import cached_property

from typing import Any, Optional, Dict, List, Pattern, Union, Type, TYPE_CHECKING
from bzd.parser.fragments import Fragment

Grammar = List[Any]


class GrammarItem:

	def __init__(self,
		regexpr: Optional[str] = None,
		fragment: Union[Type[Fragment], Dict[str, str]] = Fragment,
		grammar: Optional[Union[Grammar, str]] = None,
		checkpoint: Optional[str] = None) -> None:

		self.regexpr_ = regexpr

		if isinstance(fragment, dict):

			class SimpleFragment(Fragment):
				default = fragment.copy()  # type: ignore

			self.fragment = SimpleFragment

		elif isinstance(fragment, type(Fragment)):
			self.fragment = fragment  # type: ignore

		else:
			raise Exception("Unsupported type for GrammarItem::fragment argument: {}".format(type(fragment)))

		self.grammar = grammar
		self.checkpoint = checkpoint

	@cached_property
	def regexpr(self) -> typing.Pattern[str]:
		# Lazy precompilation of the regular expression.
		return re.compile(r"") if self.regexpr_ is None else re.compile(self.regexpr_, re.DOTALL)

	def __repr__(self) -> str:
		return str(self.regexpr)


GrammarItemSpaces = GrammarItem(r"[\s]+")
