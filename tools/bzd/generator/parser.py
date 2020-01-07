#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import yaml
from log import Log

"""
Add support for !ref tags
"""
class Ref():
	tag = u'!ref'
	def __init__(self, loader, node):
		self.value = node.value
	@staticmethod
	def representer(dumper, data):
		return dumper.represent_scalar(Ref.tag, str(data))
	def __repr__(self):
		return str(self.value)

"""
Convert the dict back to a manifest
"""
def dictToManifest(data):
	yaml.add_representer(Ref, Ref.representer)
	return yaml.dump(data)

"""
Open a manifest file and convert it to a python dict.
"""
def manifestToDict(path):

	class BzdLoader(yaml.Loader):
		pass
	BzdLoader.add_constructor(Ref.tag, Ref)

	try:
		f = open(path)
	except Exception as e:
		Log.fatal("Unable to open manifest '{}'".format(path), e)
	try:
		data = yaml.load(f, Loader = BzdLoader)
	except Exception as e:
		Log.fatal("Unable to parse manifest '{}'".format(path), e)
	finally:
		f.close()

	return data
