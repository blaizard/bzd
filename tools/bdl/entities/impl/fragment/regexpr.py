from bzd.parser.element import Element, ElementBuilder


class Regexpr:

	def __init__(self, element: Element) -> None:
		self.element = element

	@property
	def isValid(self) -> bool:
		"""Check if this is a valid regular expression."""
		return self.element.isNestedSequence("regexpr_include") or self.element.isNestedSequence("exclude_include")

	def include(self, regexpr: str) -> None:
		"""Add a new regular expression for inclusion."""
		element = ElementBuilder().setAttr("regexpr", regexpr)
		ElementBuilder.cast(self.element, ElementBuilder).pushBackElementToNestedSequence("regexpr_include", element)

	def exclude(self, regexpr: str) -> None:
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

		for include in regexpr.element.getNestedSequenceOrEmpty("regexpr_include"):
			self.include(regexpr=include.getAttr("regexpr").value)
		for exclude in regexpr.element.getNestedSequenceOrEmpty("regexpr_exclude"):
			self.exclude(regexpr=exclude.getAttr("regexpr").value)

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
