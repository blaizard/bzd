import typing

from bzd.validation.schema import Constraint
from bzd.validation.validation import (
	Schema,
)
from bzd.validation.validation import (
	SchemaDict as SchemaDict_,
)
from bzd.validation.validation import (
	SchemaList as SchemaList_,
)
from bzd.validation.validation import (
	Validation as ValidationBase,
)

from bdl.contracts.all import _Contracts

SchemaDict = SchemaDict_
SchemaList = SchemaList_


class Validation(ValidationBase[Schema]):
	AVAILABLE_CONSTRAINTS: typing.ClassVar[dict[str, type[Constraint]]] = {
		contract.name: contract.constraint for contract in _Contracts if contract.constraint
	}
