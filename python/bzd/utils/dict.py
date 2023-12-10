import typing
import enum


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


class UpdatePolicy(enum.Enum):
	override = enum.auto()
	raiseOnConflict = enum.auto()
	raiseOnNonConflict = enum.auto()


def updateDeep(d: typing.MutableMapping[typing.Any, typing.Any],
               u: typing.MutableMapping[typing.Any, typing.Any],
               policy: UpdatePolicy = UpdatePolicy.override) -> typing.MutableMapping[typing.Any, typing.Any]:
	"""Deep update of 2 dictionaries.
	
	Args:
		d: Dictionary to merge, the merge happen in-place, so this dictionary will be modified.
		u: Dictionary to apply.
		policy: The policy to be used when merging.

	Return:
		The updated dictionary.
	"""

	return _updateDeep(d, u, policy, [])


def _updateDeep(d: typing.MutableMapping[typing.Any, typing.Any], u: typing.MutableMapping[typing.Any, typing.Any],
                policy: UpdatePolicy, keys: typing.List[str]) -> typing.MutableMapping[typing.Any, typing.Any]:

	for k, v in u.items():
		keys.append(k)
		if (policy == UpdatePolicy.raiseOnNonConflict) and (k not in d):
			raise KeyError(".".join(keys))
		if isinstance(v, dict):
			d.setdefault(k, {})
			if isinstance(d[k], dict):
				d[k] = _updateDeep(d[k], v, policy, keys)
			else:
				if policy == UpdatePolicy.raiseOnConflict:
					raise KeyError(".".join(keys))
				d[k] = v
		else:
			if policy == UpdatePolicy.raiseOnConflict and (k in d):
				raise KeyError(".".join(keys))
			d[k] = v
		keys.pop()

	return d
