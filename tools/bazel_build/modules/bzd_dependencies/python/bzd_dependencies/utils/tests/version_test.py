import unittest

from bzd_dependencies.utils.version import Version


def _parse(value: str) -> Version:
	version = Version.parse(value)
	assert version is not None
	return version


class TestVersion(unittest.TestCase):
	def testParse(self) -> None:
		self.assertEqual(str(_parse("1.2.3")), "1.2.3")
		self.assertEqual(str(_parse("8.2.0.2")), "8.2.0.2")
		self.assertEqual(str(_parse("1.2.3+build.5")), "1.2.3+build.5")
		self.assertIsNone(Version.parse("v1.2.3"))
		self.assertIsNone(Version.parse("1.2.3-"))
		self.assertIsNone(Version.parse(""))

	def testIsStable(self) -> None:
		self.assertTrue(_parse("1.2.3").isStable)
		self.assertFalse(_parse("1.2.3-rc1").isStable)

	def testOrdering(self) -> None:
		self.assertGreater(_parse("0.2.27"), _parse("0.1.2"))
		self.assertGreater(_parse("1.10.0"), _parse("1.9.9"))
		self.assertLess(_parse("1.9.9"), _parse("1.10.0"))
		self.assertEqual(_parse("1.0.0"), _parse("1.0.0"))
		self.assertGreater(_parse("8.2.1.1"), _parse("8.2.1"))
		self.assertGreater(_parse("10.0.1"), _parse("8.5.1.4"))
		self.assertGreater(_parse("1.0.0"), _parse("1.0.0-rc1"))
		self.assertLess(_parse("1.0.0-rc1"), _parse("1.0.0"))
		self.assertGreater(_parse("1.0.0-alpha.2"), _parse("1.0.0-alpha.1"))
		self.assertGreater(_parse("1.0.0-alpha.1"), _parse("1.0.0-alpha"))
		self.assertGreater(_parse("1.0.0-alpha"), _parse("1.0.0-1"))
		self.assertGreater(_parse("1.0.0-10"), _parse("1.0.0-2"))
		self.assertLess(_parse("1.0.0-2"), _parse("1.0.0-10"))

	def testEqualityIgnoresBuildMetadata(self) -> None:
		self.assertEqual(_parse("1.0.0"), _parse("1.0.0+build.5"))


if __name__ == "__main__":
	unittest.main()
