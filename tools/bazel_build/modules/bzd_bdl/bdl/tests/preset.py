import json
import os
import tempfile
import typing
import unittest

from bdl.grammar import Parser
from bdl.entities.builder import ElementBuilder
from bdl.entities.impl.preset import Preset
from bdl.object import Object, ObjectContext
from bdl.entities.all import Expression


class TestRun(unittest.TestCase):
	maxDiff = 4096

	def _clearSertializedForCompare(self, sequence: typing.List[typing.Any]) -> None:
		for element in sequence:
			for key in element.keys():
				if key == "@":
					element["@"] = {key: value["v"] for key, value in element["@"].items()}
				else:
					self._clearSertializedForCompare(element[key])

	def assertParserEqual(self, parser: Parser, match: typing.List[typing.Any]) -> None:
		data = parser.parse()
		result = data.serialize()
		self._clearSertializedForCompare(result)
		self.assertListEqual(result, match)

	def testGrammar(self) -> None:
		parser = Parser(content='preset Hello from "data.json";')
		self.assertParserEqual(parser, [{"@": {"category": "preset", "name": "Hello", "value": "data.json"}}])

	def testSetContent(self) -> None:
		element = ElementBuilder("preset")
		element.setAttr("name", "test")
		element.setAttr("value", "path.json")
		preset = Preset(element)
		preset.setContent('{"hello": [{"thisinfo": 42}], "scalar": "x", "0": {"x": 1}}')
		self.assertTrue(preset.isLiteral)
		self.assertEqual(preset.json["hello"][0]["thisinfo"], 42)
		self.assertEqual(preset.get("hello.0.thisinfo"), 42)
		self.assertEqual(preset.get("0.x"), 1)
		self.assertEqual(preset.get(""), preset.json)
		self.assertEqual(preset["scalar"], "x")

	def testGetEdgeCases(self) -> None:
		element = ElementBuilder("preset")
		element.setAttr("name", "test")
		element.setAttr("value", "path.json")
		preset = Preset(element)
		preset.setContent('{"hello": [{"thisinfo": 42}], "scalar": "x"}')
		with self.assertRaisesRegex(Exception, r"not found"):
			preset.get("nonexistent")
		with self.assertRaisesRegex(Exception, r"empty segment"):
			preset.get("a..b")
		with self.assertRaisesRegex(Exception, r"non-negative"):
			preset.get("hello.-1.thisinfo")
		with self.assertRaisesRegex(Exception, r"out of bounds"):
			preset.get("hello.999.thisinfo")

	def testInvalidJSON(self) -> None:
		element = ElementBuilder("preset")
		element.setAttr("name", "test")
		element.setAttr("value", "path.json")
		preset = Preset(element)
		with self.assertRaises(Exception):
			preset.setContent("{bad json")

	def testTopLevelArray(self) -> None:
		element = ElementBuilder("preset")
		element.setAttr("name", "test")
		element.setAttr("value", "path.json")
		preset = Preset(element)
		with self.assertRaisesRegex(Exception, r"Top-level JSON arrays"):
			preset.setContent("[1, 2, 3]")

	def testResolveNotLoaded(self) -> None:
		element = ElementBuilder("preset")
		element.setAttr("name", "test")
		element.setAttr("value", "path.json")
		preset = Preset(element)
		with self.assertRaisesRegex(Exception, r"was not loaded"):
			preset.resolve(None)

	@staticmethod
	def _makeFixtureFile(content: typing.Any) -> str:
		"""Write a JSON fixture to a temp file and return its path."""
		with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False) as f:
			json.dump(content, f)
		return f.name

	def testResolveDirectAssignment(self) -> None:
		"""Direct preset assignment should error."""
		fixturePath = self._makeFixtureFile({"key": "value"})
		try:
			with self.assertRaisesRegex(Exception, r"cannot be assigned directly"):
				Object.fromContent(
					content=f'preset p from "{fixturePath}"; composition C {{ x = p; }}',
					objectContext=ObjectContext(resolve=True, composition=True),
				)
		finally:
			os.unlink(fixturePath)

	def testResolveLeafValue(self) -> None:
		"""Preset JSON path navigation should extract leaf values."""
		fixturePath = self._makeFixtureFile({"scalar": "x"})
		try:
			bdl = Object.fromContent(
				content=f'preset p from "{fixturePath}"; composition C {{ x = p.scalar; }}',
				objectContext=ObjectContext(resolve=True, composition=True),
			)
			x = bdl.entity("C.x")
			self.assertTrue(x.isLiteral)
			self.assertEqual(x.literal, '"x"')
		finally:
			os.unlink(fixturePath)

	def testResolveNonLeafError(self) -> None:
		"""Navigating to a non-leaf (container) should error."""
		fixturePath = self._makeFixtureFile({"hello": [{"thisinfo": 42}]})
		try:
			with self.assertRaisesRegex(Exception, r"non-leaf container"):
				Object.fromContent(
					content=f'preset p from "{fixturePath}"; composition C {{ x = p.hello; }}',
					objectContext=ObjectContext(resolve=True, composition=True),
				)
		finally:
			os.unlink(fixturePath)

	def testResolveMissingKey(self) -> None:
		"""Navigating to a missing key should error."""
		fixturePath = self._makeFixtureFile({"existing": "value"})
		try:
			with self.assertRaisesRegex(Exception, r"not found"):
				Object.fromContent(
					content=f'preset p from "{fixturePath}"; composition C {{ x = p.nonexistent; }}',
					objectContext=ObjectContext(resolve=True, composition=True),
				)
		finally:
			os.unlink(fixturePath)

	def testPresetExampleEndToEnd(self) -> None:
		"""Verify the preset_example.bdl file works end-to-end."""
		fixturePath = self._makeFixtureFile(
			{
				"endpoints": ["/api/v1", "/api/v2"],
				"features": {"darkMode": True, "maxItems": 100},
				"host": "10.10.0.10",
				"port": 8080,
				"version": "2.0",
			}
		)
		try:
			bdl = Object.fromContent(
				content=f'''
					namespace bzd;

					preset cfg from "{fixturePath}";
					struct Server {{
						host = String;
						port = Integer;
					}}
					composition MyService {{
						server = Server(host = cfg.host, port = cfg.port);
					}}
					''',
				objectContext=ObjectContext(resolve=True, composition=True),
			)
			server = bdl.entity("bzd.MyService.server")
			self.assertIsInstance(server, Expression)

			self.assertEqual(len(server.parametersResolved), 2)

			self.assertEqual(server.parametersResolved[0].param.literal, '"10.10.0.10"')
			self.assertEqual(server.parametersResolved[1].param.literal, "8080")
		finally:
			os.unlink(fixturePath)


if __name__ == "__main__":
	unittest.main()
