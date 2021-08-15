import typing

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import EntityType


class EntitySequence:
	"""
	Defines an entity sequence
	"""

	def __init__(self, sequence: typing.List["EntityType"]) -> None:
		self.list = sequence

	def __iter__(self) -> typing.Iterator["EntityType"]:
		for entity in self.list:
			yield entity

	def __getitem__(self, index: int) -> "EntityType":
		return self.list[index]

	def __len__(self) -> int:
		return len(self.list)

	def __repr__(self) -> str:
		return "\n".join([str(entity) for entity in self.list])
