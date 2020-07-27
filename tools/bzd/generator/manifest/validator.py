#!/usr/bin/python

import re
from .custom import Custom
from typing import Type, Any, List, Optional, Pattern, Mapping


class ValidatorType:
	valueTypes: List[Any] = []
	regexpr: Optional[Pattern[str]] = None

	@classmethod
	def isMatch(cls: Type["ValidatorType"], value: Any) -> bool:
		if str in cls.valueTypes and isinstance(value, str):
			assert cls.regexpr
			return bool(re.match(cls.regexpr, value))
		elif str in cls.valueTypes and ValidatorReference.isMatch(value):
			assert cls.regexpr
			return bool(re.match(cls.regexpr, value.value))
		else:
			for t in cls.valueTypes:
				if isinstance(value, t):
					return True
		return False


"""
Determine wether or not a value is a reference
"""


class ValidatorReference(ValidatorType):

	@classmethod
	def isMatch(cls: Type["ValidatorReference"], value: Any) -> bool:
		return hasattr(value, "__bzd_reference__") and value.__bzd_reference__ == True


"""
Determine wether this is a custome type
"""


class ValidatorCustom(ValidatorType):

	@classmethod
	def isMatch(cls: Type["ValidatorCustom"], value: Any) -> bool:
		return hasattr(value, "__bzd_custom__") and value.__bzd_custom__ == True


"""
Matches everything!
"""


class ValidatorAny(ValidatorType):

	@classmethod
	def isMatch(cls: Type["ValidatorAny"], value: Any) -> bool:
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
	def parse(cls: Type["ValidatorObject"], string: str) -> Optional[Mapping[str, str]]:
		m = re.match(cls.regexpr, str(string))
		if m:
			return {"interface": m.group("interface"), "name": m.group("name")}
		return None


"""
Validate a string
"""


class Validator():

	VALIDATORS = {
		"interface": ValidatorInterface,
		"object": ValidatorObject,
		"path": ValidatorPath,
		"any": ValidatorAny
	}

	def __init__(self, *formats: str) -> None:
		self.formats = list(formats)
		self.validators: List[Type[ValidatorType]] = []
		for f in set(formats):
			assert f in Validator.VALIDATORS, "Un-supported validator format '{}'.".format(f)
			self.validators.append(Validator.VALIDATORS[f])

	def getFormats(self) -> List[str]:
		return self.formats

	def validate(self, value: Any) -> bool:
		for validator in self.validators:
			if validator.isMatch(value):
				return True
		raise Exception("Value '{}' does not match the required type(s): {}.".format(
			str(value), ", ".join(self.formats)))
