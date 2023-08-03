import typing
import re

from bzd.parser.element import Element, ElementBuilder


class Regexpr:

	def __init__(self, element: Element) -> None:
		self.element = element

	@property
	def isValid(self) -> bool:
		"""Check if this is a valid regular expression."""
		return self.element.isNestedSequence("regexpr_include") or self.element.isNestedSequence("exclude_include")

	@property
	def includes(self) -> typing.List[str]:
		"""Get the list of includes associated with this regular expression."""
		sequence = self.element.getNestedSequenceOrEmpty("regexpr_include")
		return [e.getAttr("regexpr").value for e in sequence]

	@property
	def excludes(self) -> typing.List[str]:
		"""Get the list of excludes associated with this regular expression."""
		sequence = self.element.getNestedSequenceOrEmpty("regexpr_exclude")
		return [e.getAttr("regexpr").value for e in sequence]

	def addInclude(self, regexpr: str) -> None:
		"""Add a new regular expression for inclusion."""
		element = ElementBuilder().setAttr("regexpr", regexpr)
		ElementBuilder.cast(self.element, ElementBuilder).pushBackElementToNestedSequence("regexpr_include", element)

	def addExclude(self, regexpr: str) -> None:
		"""Add a new regular expression for exclusion."""
		element = ElementBuilder().setAttr("regexpr", regexpr)
		ElementBuilder.cast(self.element, ElementBuilder).pushBackElementToNestedSequence("regexpr_exclude", element)

	def negate(self) -> None:
		"""Move all includes to excludes and vice versa."""
		elementBuilder = ElementBuilder.cast(self.element, ElementBuilder)
		maybeSequenceInclude = elementBuilder.getNestedSequence("regexpr_include")
		maybeSequenceExclude = elementBuilder.getNestedSequence("regexpr_exclude")
		if maybeSequenceInclude is not None:
			elementBuilder.setNestedSequence("regexpr_exclude", maybeSequenceInclude)
		if maybeSequenceExclude is not None:
			elementBuilder.setNestedSequence("regexpr_include", maybeSequenceExclude)

	def merge(self, regexpr: "Regexpr") -> None:
		"""Merge another regular expression with this one."""

		for include in regexpr.includes:
			self.addInclude(regexpr=include)
		for exclude in regexpr.excludes:
			self.addExclude(regexpr=exclude)

	def assign(self, regexpr: "Regexpr") -> None:
		"""Set a regular expression."""

		self.clear()
		self.merge(regexpr=regexpr)

	def clear(self) -> None:
		"""Remove any regular expression data."""

		elementBuilder = ElementBuilder.cast(self.element, ElementBuilder)
		if self.element.isNestedSequence("regexpr_include"):
			elementBuilder.removeNestedSequence("regexpr_include")
		if self.element.isNestedSequence("regexpr_exclude"):
			elementBuilder.removeNestedSequence("regexpr_exclude")

	def match(
	    self,
	    iterable: typing.Iterable[typing.Any],
	    toString: typing.Callable[[typing.Any], str] = str,
	) -> typing.Set[typing.Any]:
		"""Match values from an iterable."""

		# Prepare the regular expressions.
		regexprInclude = re.compile("(" + "|".join(self.includes) + ")")
		regexprExclude = (re.compile("(" + "|".join(self.excludes) + ")") if self.excludes else None)

		matches = set()
		for value in iterable:
			valueAsStr = toString(value)
			if regexprInclude.match(valueAsStr):
				if not regexprExclude or not regexprExclude.match(valueAsStr):
					matches.add(value)
		return matches
