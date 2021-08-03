import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testVoid(self) -> None:

		# No arguments
		Object.fromContent(content="struct temp { var = Void; }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Void(); }", objectContext=ObjectContext(resolve=True))
		# With a value
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="struct temp { var = Void(12); }", objectContext=ObjectContext(resolve=True))
		# With template
		with self.assertRaisesRegex(Exception, r"not support template"):
			Object.fromContent(content="struct temp { var = Void<Void>; }", objectContext=ObjectContext(resolve=True))

	def testBoolean(self) -> None:

		# No arguments
		Object.fromContent(content="struct temp { var = Boolean; }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Boolean(); }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Boolean(true); }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Boolean(false); }", objectContext=ObjectContext(resolve=True))
		# With a value
		with self.assertRaisesRegex(Exception, r"expects.*boolean"):
			Object.fromContent(content="struct temp { var = Boolean(12); }", objectContext=ObjectContext(resolve=True))
		# With wrong number of arguments
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="struct temp { var = Boolean(1, 2 ,3); }",
				objectContext=ObjectContext(resolve=True))
		# With template
		with self.assertRaisesRegex(Exception, r"not support template"):
			Object.fromContent(content="struct temp { var = Boolean<Void>; }",
				objectContext=ObjectContext(resolve=True))

	def testInteger(self) -> None:

		# No arguments
		Object.fromContent(content="struct temp { var = Integer; }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Integer(); }", objectContext=ObjectContext(resolve=True))
		# With value
		Object.fromContent(content="struct temp { var = Integer(12); }", objectContext=ObjectContext(resolve=True))
		# With non-integer value
		with self.assertRaisesRegex(Exception, r"expects.*integer"):
			Object.fromContent(content="struct temp { var = Integer(32.54); }",
				objectContext=ObjectContext(resolve=True))
		# With wrong number of arguments
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="struct temp { var = Integer(1, 2 ,3); }",
				objectContext=ObjectContext(resolve=True))
		# With template
		with self.assertRaisesRegex(Exception, r"not support template"):
			Object.fromContent(content="struct temp { var = Integer<Void>; }",
				objectContext=ObjectContext(resolve=True))

	def testFloat(self) -> None:

		# No arguments
		Object.fromContent(content="struct temp { var = Float; }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Float(); }", objectContext=ObjectContext(resolve=True))
		# With value
		Object.fromContent(content="struct temp { var = Float(12); }", objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Float(-1.23); }", objectContext=ObjectContext(resolve=True))
		# With non-integer value
		with self.assertRaisesRegex(Exception, r"expects.*float"):
			Object.fromContent(content="struct temp { var = Float(\"hello\"); }",
				objectContext=ObjectContext(resolve=True))
		# With wrong number of arguments
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="struct temp { var = Float(1, 2 ,3); }",
				objectContext=ObjectContext(resolve=True))
		# With template
		with self.assertRaisesRegex(Exception, r"not support template"):
			Object.fromContent(content="struct temp { var = Float<Integer>; }",
				objectContext=ObjectContext(resolve=True))

	def testResult(self) -> None:

		# No template
		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="struct temp { var = Result; }", objectContext=ObjectContext(resolve=True))

		# Template
		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="struct temp { var = Result<Integer>; }",
				objectContext=ObjectContext(resolve=True))

		# TODO: TO BE FIXED!
		#Object.fromContent(content="struct temp { var = Result<Integer, Void>; }",
		#	objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"not a type"):
			Object.fromContent(content="struct temp { var = Result<12, Void>; }",
				objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"not a type"):
			Object.fromContent(content="struct temp { var = Result<Void, 78>; }",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="struct temp { var = Result<Integer, Void, Float>; }",
				objectContext=ObjectContext(resolve=True))

		# Template of template
		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="struct temp { var = Result<Result, Void>; }",
				objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="struct temp { var = Result<Result<Void, Void>, Void>; }",
			objectContext=ObjectContext(resolve=True))

	def testList(self) -> None:

		# No template
		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="struct temp { var = List; }", objectContext=ObjectContext(resolve=True))

		# Template
		Object.fromContent(content="struct temp { var = List<Integer>; }", objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="struct temp { var = List<Integer, Void>; }",
				objectContext=ObjectContext(resolve=True))


if __name__ == '__main__':
	unittest.main()
