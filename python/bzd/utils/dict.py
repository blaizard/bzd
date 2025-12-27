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


UpdatePolicyCallable = typing.Callable[[typing.Any, typing.Any, typing.List[str]], None]


def updateDeep(
    d: typing.MutableMapping[typing.Any, typing.Any],
    u: typing.MutableMapping[typing.Any, typing.Any],
    policy: typing.Union[UpdatePolicy, UpdatePolicyCallable] = UpdatePolicy.override
) -> typing.MutableMapping[typing.Any, typing.Any]:
	"""Deep update of 2 dictionaries.
	
	Args:
		d: Dictionary to merge, the merge happen in-place, so this dictionary will be modified.
		u: Dictionary to apply.
		policy: The policy to be used when merging.

	Return:
		The updated dictionary.
	"""

	if policy == UpdatePolicy.override:

		def policyCallback(d: typing.Any, u: typing.Any, keys: typing.List[str]) -> None:
			pass
	elif policy == UpdatePolicy.raiseOnConflict:

		def policyCallback(d: typing.Any, u: typing.Any, keys: typing.List[str]) -> None:
			if d is not None:
				raise KeyError(".".join(keys))
	elif policy == UpdatePolicy.raiseOnNonConflict:

		def policyCallback(d: typing.Any, u: typing.Any, keys: typing.List[str]) -> None:
			if d is None:
				raise KeyError(".".join(keys))
	elif callable(policy):
		policyCallback = policy  # type: ignore
	else:
		raise Exception(f"Unsupported policy '{str(policy)}'")

	return _updateDeep(d, u, policyCallback, [])


def _updateDeep(d: typing.MutableMapping[typing.Any, typing.Any], u: typing.MutableMapping[typing.Any, typing.Any],
                policyCallback: UpdatePolicyCallable,
                keys: typing.List[str]) -> typing.MutableMapping[typing.Any, typing.Any]:

	for k, v in u.items():
		keys.append(k)
		if k not in d:
			policyCallback(None, v, keys)
		if isinstance(v, dict):
			d.setdefault(k, {})
			if isinstance(d[k], dict):
				d[k] = _updateDeep(d[k], v, policyCallback, keys)
			else:
				policyCallback(d[k], v, keys)
				d[k] = v
		else:
			if k in d:
				policyCallback(d.get(k), v, keys)
			d[k] = v
		keys.pop()

	return d
