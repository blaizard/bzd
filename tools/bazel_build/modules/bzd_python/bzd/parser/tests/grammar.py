import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.tests.support.parser import TestParser


class TestRun(unittest.TestCase):

	def testFromPath(self) -> None:
		parser = TestParser("""// I am a comment
const int myVar = 42;
void hello()
{
        // I am a nested comment
        float nestedVar = 2;
}
""")
		data = parser.parse()

		result = data.serialize()
		expected = [{
		    '@': {
		        'comment': {
		            'v': 'I am a comment',
		            'i': -1,
		            'e': 0
		        },
		        'const': {
		            'v': '',
		            'i': 18,
		            'e': 23
		        },
		        'variable': {
		            'v': '',
		            'i': 24,
		            'e': 27
		        },
		        'kind': {
		            'v': 'int',
		            'i': 24,
		            'e': 27
		        },
		        'name': {
		            'v': 'myVar',
		            'i': 28,
		            'e': 33
		        },
		        'value': {
		            'v': '42',
		            'i': 36,
		            'e': 38
		        }
		    }
		}, {
		    '@': {
		        'function': {
		            'v': '',
		            'i': 40,
		            'e': 44
		        },
		        'return': {
		            'v': 'void',
		            'i': 40,
		            'e': 44
		        },
		        'name': {
		            'v': 'hello',
		            'i': 45,
		            'e': 52
		        }
		    },
		    'nested': [{
		        '@': {
		            'comment': {
		                'v': 'I am a nested comment',
		                'i': -1,
		                'e': 0
		            },
		            'variable': {
		                'v': '',
		                'i': 96,
		                'e': 101
		            },
		            'kind': {
		                'v': 'float',
		                'i': 96,
		                'e': 101
		            },
		            'name': {
		                'v': 'nestedVar',
		                'i': 102,
		                'e': 111
		            },
		            'value': {
		                'v': '2',
		                'i': 114,
		                'e': 115
		            }
		        }
		    }]
		}]

		self.assertListEqual(expected, result)

	def testFromContent(self) -> None:
		parser = TestParser("float myVar = 42;")
		data = parser.parse()

		result = data.serialize()
		expected = [{
		    '@': {
		        'variable': {
		            'v': '',
		            'i': 0,
		            'e': 5
		        },
		        'kind': {
		            'v': 'float',
		            'i': 0,
		            'e': 5
		        },
		        'name': {
		            'v': 'myVar',
		            'i': 6,
		            'e': 11
		        },
		        'value': {
		            'v': '42',
		            'i': 14,
		            'e': 16
		        }
		    }
		}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	unittest.main()
