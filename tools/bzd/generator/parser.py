#!/usr/bin/python

import bzd.yaml
from log import Log
from manifest.reference import Reference
from typing import Any, Mapping
"""
Add support for !ref tag
"""


class Ref(Reference):  # type: ignore
	tag = u'!ref'

	def __init__(self, loader: Any, node: Any) -> None:
		super(Ref, self).__init__()
		self.value = node.value

	def __repr__(self) -> str:
		return self.getRepr()  # type: ignore

	@staticmethod
	def representer(dumper: Any, data: Any) -> Any:
		return dumper.represent_scalar(Ref.tag, str(data))


"""
Convert the dict back to a manifest
"""


def dictToManifest(data: Mapping[str, Any]) -> str:
	bzd.yaml.add_representer(Ref, Ref.representer)
	return bzd.yaml.dump(data)  # type: ignore


"""
Open a manifest file and convert it to a python dict.
"""


def manifestToDict(path: str) -> Mapping[str, Any]:

	bzd.yaml.add_constructor(Ref.tag, Ref)

	try:
		f = open(path)
	except Exception as e:
		Log.fatal("Unable to open manifest '{}'".format(path), e)
	try:
		data = bzd.yaml.load(f, Loader=bzd.yaml.Loader)
	except Exception as e:
		Log.fatal("Unable to parse manifest '{}'".format(path), e)
	finally:
		f.close()

	return data  # type: ignore
