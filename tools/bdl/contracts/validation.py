import typing

from bzd.validation.validation import (
    Validation as ValidationBase,
    Schema,
    SchemaDict as SchemaDict_,
    SchemaList as SchemaList_,
)
from bzd.validation.schema import Constraint

from tools.bdl.contracts.all import _Contracts

SchemaDict = SchemaDict_
SchemaList = SchemaList_


class Validation(ValidationBase[Schema]):
	AVAILABLE_CONSTRAINTS: typing.Dict[str, typing.Type[Constraint]] = {
	    contract.name: contract.constraint
	    for contract in _Contracts if contract.constraint
	}
