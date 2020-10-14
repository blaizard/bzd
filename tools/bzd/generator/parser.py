from typing import Any, Mapping

import bzd.yaml
from tools.bzd.generator.log import Log
from tools.bzd.generator.manifest.reference import Reference

"""
Add support for !ref tag
"""


class Ref(Reference):
	tag = u'!ref'

	def __init__(self, loader: Any, node: Any) -> None:
		super(Ref, self).__init__()
		self.value = node.value

	def __repr__(self) -> str:
		return self.getRepr()

	@staticmethod
	def representer(dumper: Any, data: Any) -> Any:
		return dumper.represent_scalar(Ref.tag, str(data))


"""
Convert the dict back to a manifest
"""


def dictToManifest(data: Mapping[str, Any]) -> str:
	bzd.yaml.add_representer(Ref, Ref.representer) # type: ignore
	return bzd.yaml.dump(data)  # type: ignore


"""
Open a manifest file and convert it to a python dict.
"""


def manifestToDict(path: str) -> Mapping[str, Any]:

	bzd.yaml.add_constructor(Ref.tag, Ref) # type: ignore

	try:
		f = open(path)
	except Exception as e:
		Log.fatal("Unable to open manifest '{}'".format(path), e)
	try:
		data = bzd.yaml.load(f, Loader=bzd.yaml.Loader) # type: ignore
	except Exception as e:
		Log.fatal("Unable to parse manifest '{}'".format(path), e)
	finally:
		f.close()

	return data  # type: ignore
