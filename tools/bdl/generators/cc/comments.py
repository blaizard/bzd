import typing

from tools.bdl.entities.all import Expression
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem


def commentBlockToStr(comment: typing.Optional[str]) -> str:
	"""
    Normal block comments.
    """

	if comment is None:
		return ""
	return "".join([f"// {c}\n" for c in comment.split("\n")])


def commentEmbeddedToStr(comment: typing.Optional[str]) -> str:
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
