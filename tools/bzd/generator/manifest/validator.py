#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import re
from .custom import Custom

class ValidatorType:
	@classmethod
	def isMatch(cls, value):
		if str in cls.valueTypes and isinstance(value, str):
			return bool(re.match(cls.regexpr, value))
		elif str in cls.valueTypes and ValidatorReference.isMatch(value):
			return bool(re.match(cls.regexpr, value.value))
		else:
			for t in cls.valueTypes:
				if isinstance(value, t):
					return True
		return False

"""
Determine wether or not a value is a reference
"""
class ValidatorReference:
	@classmethod
	def isMatch(cls, value):
		return hasattr(value, "__bzd_reference__") and value.__bzd_reference__ == True

"""
Determine wether this is a custome type
"""
class ValidatorCustom:
	@classmethod
	def isMatch(cls, value):
		return hasattr(value, "__bzd_custom__") and value.__bzd_custom__ == True

"""
Matches everything!
"""
class ValidatorAny(ValidatorType):
	@classmethod
	def isMatch(cls, value):
		return True

"""
Determine the class type
"""
class ValidatorInterface(ValidatorType):
	regexprClass = r"(?P<interface>(?:(?:::)?[A-Za-z_][A-Za-z_0-9]*(?:<.*>)?)+)"
	regexpr = re.compile(r"^" + regexprClass + "$")
	valueTypes = [str]

"""
Matches on a relative path
"""
class ValidatorPath(ValidatorType):
	regexprClass = r"(?P<path>(?:(?:[^/]+/)*[^/]+))"
	regexpr = re.compile(r"^" + regexprClass + "$", re.IGNORECASE)
	valueTypes = [str]

"""
Determine the object type
"""
class ValidatorObject(ValidatorType):
	regexprObjectName = r"(?P<name>(?:[A-Za-z_][A-Za-z_0-9]*)+)"
	regexpr = re.compile(r"^" + ValidatorInterface.regexprClass + r"\." + regexprObjectName + "$")
	valueTypes = [str]

	@classmethod
	def parse(cls, string):
		m = re.match(cls.regexpr, str(string))
		if m:
			return {
				"interface": m.group("interface"),
				"name": m.group("name")
			}
		return None

"""
Validate a string
"""
class Validator():

	validators = {
		"interface": ValidatorInterface,
		"object": ValidatorObject,
		"path": ValidatorPath,
		"any": ValidatorAny
	}

	def __init__(self, *formats):
		self.formats = formats
		self.validators = []
		for f in set(formats):
			assert f in Validator.validators, "Un-supported validator format '{}'.".format(f)
			self.validators.append(Validator.validators[f])

	def getFormats(self):
		return self.formats

	def validate(self, value):
		for validator in self.validators:
			if validator.isMatch(value):
				return True
		raise Exception("Value '{}' does not match the required type(s): {}.".format(str(value), ", ".join(self.formats)))
