#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

"""
Validate a string
"""
class Validator():
	
	def __init__(self, *formats):
		self.formats = list(formats)

	def validate(self, string):
		return True
