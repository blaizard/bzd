import typing

from bdl.object import Object, ObjectContext


def preprocess(
	source: str,
	namespace: typing.Optional[typing.List[str]] = None,
	objectContext: typing.Optional[ObjectContext] = None,
) -> Object:
	objectContext = objectContext if objectContext is not None else ObjectContext()
	assert objectContext

	# Preprocess the object
	return objectContext.preprocess(source=source, namespace=namespace)
