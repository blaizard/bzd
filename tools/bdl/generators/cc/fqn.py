from tools.bdl.entities.impl.fragment.fqn import FQN


def fqnToStr(fqn: str) -> str:
	return "::".join(FQN.toNamespace(fqn))


def fqnToAdapterStr(fqn: str) -> str:
	split = FQN.toNamespace(fqn)
	split.insert(-1, "adapter")
	return "::".join(split)


def fqnToNameStr(fqn: str) -> str:
	split = FQN.toNamespace(fqn)
	return "_".join(split)
