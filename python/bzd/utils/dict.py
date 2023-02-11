import typing


def isEqual(d1: typing.MutableMapping[typing.Any, typing.Any], d2: typing.MutableMapping[typing.Any,
                                                                                         typing.Any]) -> bool:

	if len(d1.keys()) != len(d2.keys()):
		return False

	for key, value in d1.items():
		if key in d2:
			if type(value) is dict:
				if not isEqual(value, d2[key]):
					return False
			elif value != d2[key]:
				return False
		else:
			return False

	return True
