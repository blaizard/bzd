#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import re


class Filter:
    def __init__(self, filters):
        self.filters = [Filter._compile(filterStr) for filterStr in filters]

    """
	Compile a filter
	"""
    @staticmethod
    def _compile(filterStr):
        index = 0
        regexpr = r''
        while index < len(filterStr):
            c = filterStr[index]
            if c == "*":
                if index + 1 < len(filterStr) and filterStr[index + 1] == "*":
                    regexpr += r'.*'
                    index += 1
                else:
                    regexpr += r'[^/]*'
            else:
                regexpr += re.escape(c)
            index += 1
        return re.compile(regexpr + r'$')

    """
	Match a filter with a string
	"""

    def match(self, data: str):
        for regexpr in self.filters:
            if re.match(regexpr, data):
                return True
        return False
