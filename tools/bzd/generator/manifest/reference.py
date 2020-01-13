#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from .validator import ValidatorObject

"""
Represents a reference value
"""
class Reference():
	__bzd_reference__ = True
	def isObject(self):
		return ValidatorObject.isMatch(self)
	def getRepr(self):
		#if self.isObject():
		#	pass
		return str(self.value)
