from tools.bdl.visitors.symbol_map import SymbolMap


def fqnToStr(fqn: str) -> str:
	return "::".join(SymbolMap.FQNToNamespace(fqn))


def fqnToAdapterStr(fqn: str) -> str:
	split = SymbolMap.FQNToNamespace(fqn)
	split.insert(-1, "adapter")
	return "::".join(split)


def fqnToNameStr(fqn: str) -> str:
	split = SymbolMap.FQNToNamespace(fqn)
	return "_".join(split)
