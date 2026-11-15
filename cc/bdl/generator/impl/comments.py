from bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem


def commentBlockToStr(comment: str | None) -> str:
	"""
	Normal block comments.
	"""

	if comment is None:
		return ""
	return "".join([f"// {c}\n" for c in comment.split("\n")])


def commentEmbeddedToStr(comment: str | None) -> str:
	"""
	Inline and embedded comments.
	"""

	if comment is None:
		return ""
	commentSplit = comment.split("\n")
	return f"/* {commentSplit} */"


def commentParametersResolvedToStr(item: ParametersResolvedItem) -> str:
	"""
	Create an inline comment from a parameter resolved entry.
	"""

	return f"/*{item.name}*/" if item.isName else ""
