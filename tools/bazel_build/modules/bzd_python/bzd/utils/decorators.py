import typing


class cached_classproperty(object):

	def __init__(self, method: typing.Any = None) -> None:
		self.fget = method

	def get_result_field_name(self) -> typing.Optional[str]:
		return self.fget.__name__ + "_property_result" if self.fget else None

	def __get__(self, instance: typing.Any, cls: typing.Any = None) -> typing.Any:
		result_field_name = self.get_result_field_name()

		if not cls or not result_field_name:
			return self.fget(cls)

		if hasattr(cls, result_field_name):
			return getattr(cls, result_field_name)

		setattr(cls, result_field_name, self.fget(cls))
		return getattr(cls, result_field_name)

	def getter(self, method: typing.Any) -> typing.Any:
		self.fget = method
		return self
