import typing


def isEqual(
    d1: typing.MutableMapping[typing.Any, typing.Any],
    d2: typing.MutableMapping[typing.Any, typing.Any],
) -> bool:
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


def updateDeep(
    d: typing.MutableMapping[typing.Any, typing.Any],
    u: typing.MutableMapping[typing.Any, typing.Any],
) -> typing.MutableMapping[typing.Any, typing.Any]:
	"""Deep update of 2 dictionaries."""

	for k, v in u.items():
		if isinstance(v, dict):
			d[k] = updateDeep(d.get(k, {}), v)
		else:
			d[k] = v
	return d
