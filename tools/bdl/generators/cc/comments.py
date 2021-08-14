import typing


def commentBlockToStr(comment: typing.Optional[str]) -> str:
	"""
	Normal block comments.
	"""

	if comment is None:
		return ""
	commentSplit = comment.split("\n")
	if len(commentSplit) > 1:
		return "/*\n{comment}\n */\n".format(comment="\n".join([" * {}".format(line) for line in commentSplit]))
	return "// {comment}\n".format(comment=comment)


def commentEmbeddedToStr(comment: typing.Optional[str]) -> str:
	"""
	Inline and embedded comments.
	"""

	if comment is None:
		return ""
	commentSplit = comment.split("\n")
	return "/* {comment} */".format(comment=" ".join(commentSplit))
