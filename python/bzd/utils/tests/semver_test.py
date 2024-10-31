import unittest
from bzd.utils.semver import Semver, SemverMatcher


class TestSemver(unittest.TestCase):

	def testSemver(self) -> None:
		self.assertEqual(str(Semver.fromString("1.0.0")), "1.0.0")
		self.assertEqual(str(Semver.fromString("1")), "1.0.0")
		self.assertEqual(str(Semver.fromString("1-beta")), "1.0.0-beta")
		self.assertEqual(Semver.fromString("1-beta").core, "1.0.0")
		self.assertEqual(Semver.fromString("1-beta").preRelease, "beta")
		self.assertEqual(Semver.fromString("1.1.1+123").build, "123")
		self.assertEqual(Semver.fromString("1.1.1-beta+123").preRelease, "beta")
		self.assertEqual(Semver.fromString("1.1.1-beta+123").build, "123")

		# Comparison operators.

		self.assertEqual(Semver.fromString("1-beta"), Semver.fromString("1-beta"))
		self.assertEqual(Semver.fromString("1.0.0-beta"), Semver.fromString("1-beta"))
		self.assertLess(Semver.fromString("1.0.0"), Semver.fromString("1.0.1"))
		self.assertGreater(Semver.fromString("2.0.0"), Semver.fromString("1.0.1"))
		self.assertGreaterEqual(Semver.fromString("2.0.0"), Semver.fromString("2"))

		# Test cases from https://semver.org/spec/v2.0.0-rc.2.html#spec-item-11
		self.assertLess(Semver.fromString("1.0.0-alpha"), Semver.fromString("1.0.0-alpha.1"))
		self.assertLess(Semver.fromString("1.0.0-alpha.1"), Semver.fromString("1.0.0-beta.2"))
		self.assertLess(Semver.fromString("1.0.0-beta.2"), Semver.fromString("1.0.0-beta.11"))
		self.assertLess(Semver.fromString("1.0.0-beta.11"), Semver.fromString("1.0.0-rc.1"))
		self.assertLess(Semver.fromString("1.0.0-rc.1"), Semver.fromString("1.0.0"))

		# Sorting
		self.assertEqual(
		    [Semver.fromString("1.0.0-alpha"),
		     Semver.fromString("1.0.0-beta.11"),
		     Semver.fromString("1.0.0")],
		    sorted([Semver.fromString("1.0.0-beta.11"),
		            Semver.fromString("1.0.0-alpha"),
		            Semver.fromString("1.0.0")]))

	def testSemverMatcherTilde(self) -> None:

		self.assertTrue(SemverMatcher("~1.0.0").match(Semver.fromString("1.0.0")))
		self.assertTrue(SemverMatcher("~1.0.0").match(Semver.fromString("1.0.9")))
		self.assertFalse(SemverMatcher("~1.0.0").match(Semver.fromString("1.1.0")))

		# >= 1.2.3, < 1.3.0
		self.assertTrue(SemverMatcher("~1.2.3").match(Semver.fromString("1.2.4")))
		self.assertFalse(SemverMatcher("~1.2.3").match(Semver.fromString("1.2.2")))
		self.assertFalse(SemverMatcher("~1.2.3").match(Semver.fromString("1.3.0")))

		# >= 1, < 2
		self.assertTrue(SemverMatcher("~1").match(Semver.fromString("1.2.4")))
		self.assertFalse(SemverMatcher("~1").match(Semver.fromString("0.9")))
		self.assertFalse(SemverMatcher("~1").match(Semver.fromString("2")))

		# >= 2.3, < 2.4
		self.assertTrue(SemverMatcher("~2.3").match(Semver.fromString("2.3.5")))
		self.assertFalse(SemverMatcher("~2.3").match(Semver.fromString("2.2.9")))
		self.assertFalse(SemverMatcher("~2.3").match(Semver.fromString("2.4")))

		# >= 1.2.0, < 1.3.0
		self.assertTrue(SemverMatcher("~1.2.x").match(Semver.fromString("1.2.5")))
		self.assertFalse(SemverMatcher("~1.2.x").match(Semver.fromString("1.1")))
		self.assertFalse(SemverMatcher("~1.2.x").match(Semver.fromString("1.3")))

		# >= 1, < 2
		self.assertTrue(SemverMatcher("~1.x").match(Semver.fromString("1.5.5")))
		self.assertFalse(SemverMatcher("~1.x").match(Semver.fromString("0.8")))
		self.assertFalse(SemverMatcher("~1.x").match(Semver.fromString("2")))

	def testSemverMatcherCaret(self) -> None:

		# >= 1.2.3, < 2.0.0
		self.assertTrue(SemverMatcher("^1.2.3").match(Semver.fromString("1.4.4")))
		self.assertFalse(SemverMatcher("^1.2.3").match(Semver.fromString("1.2.2")))
		self.assertFalse(SemverMatcher("^1.2.3").match(Semver.fromString("2.0.0")))

		# >= 1.2.0, < 2.0.0
		self.assertTrue(SemverMatcher("^1.2.x").match(Semver.fromString("1.4.4")))
		self.assertFalse(SemverMatcher("^1.2.x").match(Semver.fromString("1.1.9")))
		self.assertFalse(SemverMatcher("^1.2.x").match(Semver.fromString("2.0.0")))

		# >= 2.3, < 3
		self.assertTrue(SemverMatcher("^2.3").match(Semver.fromString("2.9")))
		self.assertFalse(SemverMatcher("^2.3").match(Semver.fromString("2.2")))
		self.assertFalse(SemverMatcher("^2.3").match(Semver.fromString("3")))

		# >= 2.0.0, < 3
		self.assertTrue(SemverMatcher("^2.x").match(Semver.fromString("2.9")))
		self.assertFalse(SemverMatcher("^2.x").match(Semver.fromString("1.9")))
		self.assertFalse(SemverMatcher("^2.x").match(Semver.fromString("3")))

		# >=0.2.3 <0.3.0
		self.assertTrue(SemverMatcher("^0.2.3").match(Semver.fromString("0.2.8")))
		self.assertFalse(SemverMatcher("^0.2.3").match(Semver.fromString("0.2.2")))
		self.assertFalse(SemverMatcher("^0.2.3").match(Semver.fromString("0.3")))

		# >=0.2.0 <0.3.0
		self.assertTrue(SemverMatcher("^0.2").match(Semver.fromString("0.2.8")))
		self.assertFalse(SemverMatcher("^0.2").match(Semver.fromString("0.1.9")))
		self.assertFalse(SemverMatcher("^0.2").match(Semver.fromString("0.3")))

		# >=0.0.3 <0.0.4
		self.assertTrue(SemverMatcher("^0.0.3").match(Semver.fromString("0.0.3")))
		self.assertFalse(SemverMatcher("^0.0.3").match(Semver.fromString("0.0.2")))
		self.assertFalse(SemverMatcher("^0.0.3").match(Semver.fromString("0.0.4")))

		# >=0.0.0 <0.1.0
		self.assertTrue(SemverMatcher("^0.0").match(Semver.fromString("0.0.9")))
		self.assertTrue(SemverMatcher("^0.0").match(Semver.fromString("0.0.0")))
		self.assertFalse(SemverMatcher("^0.0").match(Semver.fromString("0.1.0")))

		# >=0.0.0 <1.0.0
		self.assertTrue(SemverMatcher("^0").match(Semver.fromString("0.9.9")))
		self.assertTrue(SemverMatcher("^0").match(Semver.fromString("0.0.0")))
		self.assertFalse(SemverMatcher("^0").match(Semver.fromString("1.0.0")))

	def testSemverMatcherOperator(self) -> None:

		# >= 1.2.0
		self.assertTrue(SemverMatcher(">=1.2.x").match(Semver.fromString("1.2.0")))
		self.assertTrue(SemverMatcher(">=1.2.x").match(Semver.fromString("9")))
		self.assertFalse(SemverMatcher(">=1.2.x").match(Semver.fromString("1.1")))

		# < 3
		self.assertTrue(SemverMatcher("<= 2.x").match(Semver.fromString("2.9")))
		self.assertTrue(SemverMatcher("<= 2.x").match(Semver.fromString("0")))
		self.assertFalse(SemverMatcher("<= 2.x").match(Semver.fromString("3")))

	def testSemverMatcherNoOperator(self) -> None:

		# >= 1.2.0, < 1.3.0
		self.assertTrue(SemverMatcher("1.2.x").match(Semver.fromString("1.2.0")))
		self.assertTrue(SemverMatcher("1.2.x").match(Semver.fromString("1.2.9")))
		self.assertFalse(SemverMatcher("1.2.x").match(Semver.fromString("1.3")))
		self.assertFalse(SemverMatcher("1.2.x").match(Semver.fromString("1.1.9")))

		# >= 0.0.0
		self.assertTrue(SemverMatcher("*").match(Semver.fromString("1.2.0")))
		self.assertTrue(SemverMatcher("*").match(Semver.fromString("0")))
		self.assertTrue(SemverMatcher("*").match(Semver.fromString("99999999")))

	def testSemverMatcherMultiple(self) -> None:

		# or operator.
		self.assertTrue(SemverMatcher("1.1 || 1.2").match(Semver.fromString("1.2.0")))
		self.assertTrue(SemverMatcher("1.1 || 1.2 || 3.4 || 1.5").match(Semver.fromString("3.4")))
		self.assertFalse(SemverMatcher("1.1 || 1.2").match(Semver.fromString("3.4")))

		# and operator.
		self.assertTrue(SemverMatcher(">=1.1 <=1.2").match(Semver.fromString("1.2.0")))
		self.assertFalse(SemverMatcher(">=1.1 <=1.2").match(Semver.fromString("1.3")))
		self.assertTrue(SemverMatcher(">=1.1 <=3.4").match(Semver.fromString("2.1")))

		# Reuse the same instance.
		matcher = SemverMatcher(">=1.1 <=1.2")
		self.assertTrue(matcher.match(Semver.fromString("1.2.0")))
		self.assertTrue(matcher.match(Semver.fromString("1.1.0")))
		self.assertTrue(matcher.match(Semver.fromString("1.1.2")))
		self.assertFalse(matcher.match(Semver.fromString("1.0.2")))
		self.assertFalse(matcher.match(Semver.fromString("1.3.2")))


if __name__ == "__main__":
	unittest.main()
