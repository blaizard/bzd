import json
import typing
from pathlib import Path

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error

from bdl.entities.impl.entity import EntityExpression, Role

if typing.TYPE_CHECKING:
	from bdl.visitors.symbol_map import Resolver


class Preset(EntityExpression):
	"""
	A preset statement loads a JSON file and exposes its content as a value.
	- Attributes:
	        - name: The resolvable symbol referring to this preset.
	        - value: The JSON file path, workspace/execroot-relative (like `use`).
	        - literal: Canonicalized JSON content (set during preprocess).
	- Role: Meta (untyped value blob; never modeled as real code).
	"""

	def __init__(self, element: Element) -> None:
		super().__init__(element, Role.Meta)
		Error.assertHasAttr(element=element, attr="name")
		Error.assertHasAttr(element=element, attr="value")

	@property
	def path(self) -> Path:
		return Path(self.element.getAttr("value").value)

	def setContent(self, content: str) -> None:
		try:
			parsed = json.loads(content)
		except json.JSONDecodeError as e:
			self.error(message=f"Invalid JSON in preset '{self.name}' ('{self.path}'): {e}")
			return
		self.assertTrue(
			condition=not isinstance(parsed, list),
			message=f"Top-level JSON arrays are not supported for Preset '{self.name}'.",
		)
		ElementBuilder.cast(self.element, ElementBuilder).setAttr("literal", json.dumps(parsed, sort_keys=True))

	@property
	def json(self) -> typing.Any:
		self.assertTrue(self.isLiteral, f"Preset '{self.name}' has no loaded content.")
		return json.loads(self.literal)  # type: ignore

	def get(self, path: str) -> typing.Any:
		current = self.json
		if path == "":
			return current
		segments = path.split(".")
		self.assertTrue(
			condition=all(seg != "" for seg in segments),
			message=f"Malformed access path '{path}' for Preset '{self.name}': empty segment.",
		)
		for part in segments:
			if isinstance(current, list):
				self.assertTrue(
					condition=part.isdigit() and int(part) >= 0,
					message=f"Expected non-negative integer index, got '{part}' in '{path}' for Preset '{self.name}'.",
				)
				index = int(part)
				try:
					current = current[index]
				except IndexError:
					self.error(f"Index {index} out of bounds in Preset '{self.name}' for '{path}'.")
			elif isinstance(current, dict):
				self.assertTrue(
					condition=part in current,
					message=f"Key '{part}' not found in Preset '{self.name}' for '{path}'.",
				)
				current = current[part]
			else:
				self.error(f"Cannot navigate '{part}' in Preset '{self.name}': not a container.")
		return current

	def getLeaf(self, path: str) -> str:
		"""Navigate a JSON path and return a JSON-encoded scalar leaf value.

		Only scalars (str, int, float, bool, None) are assignable.
		Raises if the path resolves to a non-leaf container.
		"""
		value = self.get(path)
		self.assertTrue(
			condition=not isinstance(value, (dict, list)),
			message=f"Path '{path}' for Preset '{self.name}' resolves to a non-leaf container, only leaf values are assignable.",
		)
		return json.dumps(value)

	def _resolveTrailingFQN(
		self,
		resolver: "Resolver",
		remaining: typing.List[str],
	) -> typing.Optional[str]:
		"""Handle unresolved trailing FQN segments by navigating the JSON structure."""
		self.assertTrue(
			condition=bool(remaining),
			message=f"Preset '{self.name}' cannot be assigned directly, navigate to a JSON leaf.",
		)
		self.resolveMemoized(resolver=resolver)
		return self.getLeaf(".".join(remaining))

	def __getitem__(self, key: typing.Union[str, int]) -> typing.Any:
		return self.json[key]

	def resolve(self, resolver: "Resolver") -> None:
		self.assertTrue(
			condition=self.isLiteral,
			message=f"Preset '{self.name}' was not loaded: '{self.path}'.",
		)
		# Intentionally not setting fqn_type: Preset is an untyped value blob.
		super().resolve(resolver)

	def __repr__(self) -> str:
		return self.toString({"name": self.name, "path": self.path.as_posix()})
