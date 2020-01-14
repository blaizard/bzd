#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

"""
Represents a reference value
"""
class Reference(object):
	__bzd_reference__ = True
	def __init__(self):
		self.reprCallback = None
		self.manifest = None
	def getRepr(self):
		return self.reprCallback(self.manifest.renderer) if self.reprCallback and self.manifest.renderer else str(self.value)
	def setReprCallback(self, manifest, callback):
		assert self.reprCallback == None, "Repr callback has already been set for this value: {}.".format(str(self.value))
		self.reprCallback = callback
		self.manifest = manifest
