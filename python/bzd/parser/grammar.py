import re
import typing
from functools import cached_property
from typing import Any

from bzd.parser.fragments import Fragment

Grammar = list[Any]


class GrammarItem:
	def __init__(
		self,
		regexpr: str | None = None,
		fragment: type[Fragment] | dict[str, str] = Fragment,
		grammar: Grammar | str | None = None,
		checkpoint: str | None = None,
	) -> None:
		self.regexpr_ = regexpr

		if isinstance(fragment, dict):

			class SimpleFragment(Fragment):
				default = fragment.copy()  # type: ignore

			self.fragment = SimpleFragment

		elif isinstance(fragment, type(Fragment)):
			self.fragment = fragment  # type: ignore

		else:
			raise TypeError(f"Unsupported type for GrammarItem::fragment argument: {type(fragment)}")

		self.grammar = grammar
		self.checkpoint = checkpoint

	@cached_property
	def regexpr(self) -> typing.Pattern[str]:
		# Lazy precompilation of the regular expression.
		return re.compile(r"") if self.regexpr_ is None else re.compile(self.regexpr_, re.DOTALL)

	def __repr__(self) -> str:
		return str(self.regexpr)


GrammarItemSpaces = GrammarItem(r"[\s]+")
