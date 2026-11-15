from bdl.object import Object, ObjectContext


def preprocess(
	source: str,
	namespace: list[str] | None = None,
	objectContext: ObjectContext | None = None,
) -> Object:
	objectContext = objectContext if objectContext is not None else ObjectContext()
	assert objectContext

	# Preprocess the object
	return objectContext.preprocess(source=source, namespace=namespace)
