import typing


class classproperty(object):

	def __init__(self, fget: typing.Any) -> None:
		self.fget = fget

	def __get__(self, obj: typing.Any, owner: typing.Any = None) -> typing.Any:
		if owner is None:
			owner = type(obj)
		return self.fget.__get__(obj, owner)()
