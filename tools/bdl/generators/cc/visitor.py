import typing
import json
from pathlib import Path

from bzd.template.template import Template

from tools.bdl.visitors.composition.visitor import CompositionView, AsyncType as AsyncTypeOriginal
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using, Expression
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved, ParametersResolvedItem
from tools.bdl.entities.impl.types import Category as CategoryOriginal

from tools.bdl.generators.cc.symbol import symbolToStr as symbolToStrOriginal
from tools.bdl.generators.cc.value import valueToStr as valueToStrOriginal
from tools.bdl.generators.cc.comments import commentBlockToStr as commentBlockToStrOriginal, commentEmbeddedToStr as commentEmbeddedToStrOriginal, commentParametersResolvedToStr as commentParametersResolvedToStrOriginal
from tools.bdl.generators.cc.fqn import fqnToStr as fqnToStrOriginal, fqnToAdapterStr as fqnToAdapterStrOriginal, fqnToNameStr as fqnToNameStrOriginal, fqnToCapitalized as fqnToCapitalizedOriginal
"""
Use cases:

Interfaces: (abstract notion)
- component name : inteface {}  <- Should be set as a normal type.
- var = interface&              <- When used it should always be as a reference.

Expressions:
- Can be defined:
	- var = Type{1};
	- var = MyComponent{2};
- Can be used in config:
	- var = MyInterface&;
	- literals are static constexpr.
	- Anything that reference a variable is reference.
	- Builtins and struct are copy.
- Can be used in function parameters (declaration):
	- 
"""


# String related
class Transform:

	Category = CategoryOriginal
	AsyncType = AsyncTypeOriginal

	def __init__(self,
	             composition: typing.Optional[CompositionView] = None,
	             data: typing.Optional[Path] = None) -> None:
		self.composition = composition
		self.data = json.loads(data.read_text()) if data else {}

	def toCamelCase(self, string: str) -> str:
		assert len(string), "String cannot be empty."
		return string[0].upper() + string[1:]

	# Inheritance related

	def inheritanceToStr(self, inheritanceList: typing.List[Symbol]) -> str:
		return ", ".join([f"public {str(symbolToStrOriginal(inheritance))}<Impl>" for inheritance in inheritanceList])

	def inheritanceAdapterToStr(self, inheritanceList: typing.List[Symbol]) -> str:
		return ", ".join([
		    "public {}<Impl>".format(str(symbolToStrOriginal(inheritance, adapter=True)))
		    for inheritance in inheritanceList
		])

	# Path related

	def bdlPathToHeader(self, path: Path) -> str:
		return path.as_posix().replace(".bdl", ".hh")

	# Namespace related

	def namespaceToStr(self, entity: Namespace) -> str:
		return "::".join(entity.nameList)

	# Symbol related

	def symbolToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol)

	def symbolToNameStr(self, symbol: Symbol) -> str:
		split = symbol.fqn.split(";")
		fqn = [split[0]] + [fqn.split(".")[-1] for fqn in split[1:]]
		return fqnToNameStrOriginal(".".join(fqn))

	def symbolReferenceToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, reference=True, referenceForInterface=True)

	def symbolDefinitionToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, definition=True, referenceForInterface=True)

	def symbolNonConstToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, nonConst=True)

	def symbolRegistryToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(
		    symbol=symbol, registry=self.composition.registry.keys() if self.composition else None)  # type: ignore

	# ParameterResolvedItem related

	def paramsConstructorDefinition(self, name: str, params: ParametersResolved) -> str:
		args = []
		assigns = []
		for index, item in enumerate(params):
			if item.param.isLiteral:
				pass
			elif item.isLValue:
				args.append(f"T{index}& {item.name}_")
				assigns.append(f"{item.name}{{{item.name}_}}")
			elif item.isRValue:
				args.append(f"T{index}&& {item.name}_")
				assigns.append(f"{item.name}{{bzd::move({item.name}_)}}")
		if not args:
			return f"{name}() noexcept = default;"
		return f"constexpr {name}({', '.join(args)}) noexcept : {', '.join(assigns)} {{}}"

	def paramsTemplateDefinition(self, params: ParametersResolved) -> str:
		args = []
		for index, item in enumerate(params):
			if not item.param.isLiteral:
				args.append(f"class T{index}")
		if not args:
			return ""
		return f"template <{', '.join(args)}>"

	def paramToDefinition(self, item: ParametersResolvedItem, index: int) -> str:

		if item.param.isLiteral:
			return f"static constexpr {symbolToStrOriginal(item.symbol)} {item.name}{{{item.param.literal}}};"
		elif item.isLValue:
			return f"T{index}& {item.name};"
		elif item.isRValue:
			return f"T{index} {item.name};"
		item.error(message="Type not supported, should never happen.")
		return ""

	def paramsDeclarationToList_(self, params: ParametersResolved, isRegistry: bool = False) -> typing.List[str]:
		"""Declare inline parameters.

		Args:
			params: The parameters to be serialized.
			isRegistry: Use registry variable instead of direct variables.
		"""
		registry = self.composition.registry.keys() if self.composition and isRegistry else None
		symbols = self.composition.symbols if self.composition else None
		return [valueToStrOriginal(item, symbols=symbols, registry=registry) for item in params]  # type: ignore

	def paramsDeclaration(self, params: ParametersResolved, isRegistry: bool = False) -> str:
		return ", ".join(self.paramsDeclarationToList_(params=params, isRegistry=isRegistry))

	def paramsFilterOutLiterals(self, params: ParametersResolved) -> typing.Iterator[ParametersResolvedItem]:
		for item in params:
			if item.param.isLiteral:
				continue
			yield item

	# Expression related

	def expressionToValue(self, entity: Expression) -> str:
		return valueToStrOriginal(item=entity.toParametersResolvedItem())

	def expressionToDefinition(self, entity: Expression) -> str:
		"""
		normal           -> int myvar{value}
		no default value -> int myvar
		unamed           -> int{value}
		"""
		output = ""
		values = self.paramsDeclarationToList_(
		    params=entity.parametersResolved) if entity.parametersResolved.size() else []
		entity.assertTrue(condition=entity.isSymbol, message="An expression declaration must have a type.")
		output += f" {symbolToStrOriginal(entity.symbol, referenceForInterface=True, values=values)}"
		if entity.isName:
			output += f" {entity.name}"
		if values:
			output += f"{{{', '.join(values)}}}"

		return output

	# Comments

	def commentBlockToStr(self, comment: str) -> str:
		return commentBlockToStrOriginal(comment=comment)

	def commentEmbeddedToStr(self, comment: str) -> str:
		return commentEmbeddedToStrOriginal(comment=comment)

	def commentParametersResolvedToStr(self, item: ParametersResolvedItem) -> str:
		return commentParametersResolvedToStrOriginal(item=item)

	# FQNs

	def fqnToStr(self, fqn: str) -> str:
		return fqnToStrOriginal(fqn=fqn)

	def fqnToAdapterStr(self, fqn: str) -> str:
		return fqnToAdapterStrOriginal(fqn=fqn)

	def fqnToNameStr(self, fqn: str) -> str:
		return fqnToNameStrOriginal(fqn=fqn)

	def fqnToCapitalized(self, fqn: str) -> str:
		"""Convert an FQN into a capitalized name."""
		return fqnToCapitalizedOriginal(fqn=fqn)

	# Async type

	def asyncTypeToStr(self, asyncType: AsyncTypeOriginal) -> str:
		return {
		    AsyncTypeOriginal.workload: "bzd::async::Type::workload",
		    AsyncTypeOriginal.service: "bzd::async::Type::service",
		}[asyncType]

	# Parameter
	def parametersResolvedToStr(self, expression: Expression) -> str:
		if expression.element.isAttr("key"):
			return expression.element.getAttr("key").value
		return ""

	# Connection
	def connectionCount(self, connection: typing.Any) -> int:
		count = 1
		for output in connection.get("readers", []):
			count = max(count, output.get("history", 1))
		return count + 1

	def registryConnectionsDeclaration(self, connections: typing.Any) -> str:

		def getBufferName(identifier: str) -> str:
			return "io_buffer_" + self.fqnToNameStr(identifier)

		args = []
		factoryTypes = {"reader": "makeReader()", "writer": "makeWriter()"}
		factoryStubTypes = {"reader": "bzd::io::ReaderStub", "writer": "bzd::io::WriterStub"}
		for name, metadata in connections.items():

			kind = metadata["type"]

			# if multi arguments, create a tuple.
			if metadata["multi"]:
				params = []
				for identifier in metadata["connections"]:
					params.append(f"{getBufferName(identifier)}.{factoryTypes[kind]}")
				arg = f"bzd::makeTuple({', '.join(params)})"
			# Use stubs if no connection exists.
			elif len(metadata["connections"]) == 0:
				arg = f"{factoryStubTypes[kind]}<{self.symbolToStr(metadata['symbol'])}>()"
			else:
				if kind == "reader":
					identifier = metadata["connections"][0]
					assert len(metadata["connections"]), f"Must be only one connection, {metadata['connections']}"
				else:
					identifier = metadata["identifier"]
				arg = f"{getBufferName(identifier)}.{factoryTypes[kind]}"

			args.append(f"/*{name}*/{arg}")

		return ", ".join(args)

	# Generic iterable

	def iterableConstructorDefinitionRValue(self, name: str, it: typing.Iterable[str]) -> str:
		args = []
		assigns = []
		for index, variableName in enumerate(it):
			args.append(f"T{index}&& {variableName}_")
			assigns.append(f"{variableName}{{bzd::move({variableName}_)}}")
		if not args:
			return f"{name}() noexcept = default;"
		return f"constexpr {name}({', '.join(args)}) noexcept : {', '.join(assigns)} {{}}"

	def iterableTemplateDefinition(self, it: typing.Iterable[typing.Any]) -> str:
		args = []
		for index, item in enumerate(it):
			args.append(f"class T{index}")
		if not args:
			return ""
		return f"template <{', '.join(args)}>"


def formatCc(bdl: Object, data: typing.Optional[Path] = None) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/file.h.btl", indent=True)
	output = template.render(bdl.tree, Transform(data=data))

	return output


def compositionCc(compositions: typing.Dict[str, CompositionView],
                  output: Path,
                  data: typing.Optional[Path] = None) -> None:

	template = Template.fromPath(Path(__file__).parent / "template/composition.cc.btl", indent=True)

	for target, composition in compositions.items():
		content = template.render(composition, Transform(composition=composition, data=data))
		(output.parent / f"{output.name}.{target}.cc").write_text(content)
