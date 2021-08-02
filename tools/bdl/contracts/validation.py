import typing

from bzd.validation.validation import Validation as ValidationBase
from bzd.validation.schema import Constraint

from tools.bdl.contracts.all import _Contracts


class Validation(ValidationBase):
	AVAILABLE_CONSTRAINTS: typing.Dict[str, typing.Type[Constraint]] = {
		contract.name: contract.constraint
		for contract in _Contracts if contract.constraint
	}
