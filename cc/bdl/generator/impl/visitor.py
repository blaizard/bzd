import typing
import json
from pathlib import Path

from bzd.template.template import Template

from bdl.visitors.composition.visitor import CompositionView
from bdl.visitors.composition.components import EntryType
from bdl.object import Object
from bdl.entities.all import Namespace, Using, Expression
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.parameters_resolved import (
    ParametersResolved,
    ParametersResolvedItem,
)
from bdl.entities.impl.types import Category as CategoryOriginal

from cc.bdl.generator.impl.symbol import symbolToStr as symbolToStrOriginal
from cc.bdl.generator.impl.value import valueToStr as valueToStrOriginal, literalNativeToStr
from cc.bdl.generator.impl.comments import (
    commentBlockToStr as commentBlockToStrOriginal,
    commentEmbeddedToStr as commentEmbeddedToStrOriginal,
    commentParametersResolvedToStr as commentParametersResolvedToStrOriginal,
)
from cc.bdl.generator.impl.fqn import (
    fqnToStr as fqnToStrOriginal,
    fqnToAdapterStr as fqnToAdapterStrOriginal,
    fqnToNameStr as fqnToNameStrOriginal,
    fqnToCapitalized as fqnToCapitalizedOriginal,
)
"""
Use cases:

Interfaces: (abstract notion)
- component name : interface {}  <- Should be set as a normal type.
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

	def __init__(
	    self,
	    composition: typing.Optional[CompositionView] = None,
	    data: typing.Optional[Path] = None,
	) -> None:
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
		split = symbol.kinds
		fqn = [split[0]] + [fqn.split(".")[-1] for fqn in split[1:]]
		return fqnToNameStrOriginal(".".join(fqn))

	def symbolReferenceToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, reference=True)

	def symbolDefinitionToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, definition=True)

	def symbolNonConstToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, nonConst=True)

	def symbolRegistryToStr(self, symbol: Symbol) -> str:
		return symbolToStrOriginal(symbol=symbol, registry=True)

	# ParameterResolvedItem related

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
			return f"static constexpr {symbolToStrOriginal(item.symbol)} {item.name}{{{literalNativeToStr(item.param.literalNative)}}};"
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
		symbols = self.composition.symbols if self.composition else None
		return [valueToStrOriginal(item, symbols=symbols, registry=isRegistry) for item in params]

	def paramsDesignatedInitializers(self, params: ParametersResolved) -> str:
		"""Initializing an aggregate with designated initializers.

        Note, it must have named parameters.
        Something like this:
        `.hello = "here", .world = 2`
        """

		values = {item.name: valueToStrOriginal(item, includeComment=False) for item in params}
		valueToList = [f".{key} = {value}" for key, value in values.items()]
		if len(valueToList) < 2:
			return ", ".join(valueToList)
		return ",\n".join(valueToList)

	def paramsDeclaration(self, params: ParametersResolved, isRegistry: bool) -> str:
		paramsToList = self.paramsDeclarationToList_(params=params, isRegistry=isRegistry)
		if len(paramsToList) < 2:
			return ", ".join(paramsToList)
		return ",\n".join(paramsToList)

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
        unnamed           -> int{value}
        """
		output = ""
		values = (self.paramsDeclarationToList_(
		    params=entity.parametersResolved) if entity.parametersResolved.size() else [])
		entity.assertTrue(
		    condition=entity.isSymbol,
		    message="An expression declaration must have a type.",
		)
		output += f" {symbolToStrOriginal(entity.symbol, values=values)}"
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

	def asyncTypeToStr(self, asyncType: EntryType) -> str:
		return {
		    EntryType.workload: "bzd::async::Type::workload",
		    EntryType.service: "bzd::async::Type::service",
		}[asyncType]

	# Parameter
	def parametersResolvedToStr(self, expression: Expression) -> str:
		if expression.element.isAttr("key"):
			return expression.element.getAttr("key").value
		return ""

	# Connection
	def connectionCount(self, connection: typing.Any) -> int:
		count = 1
		for output in connection.get("sinks", []):
			count = max(count, output.get("history", 1))
		return count + 1

	def registryConnectionsDeclaration(self, connections: typing.Any) -> str:

		def getBufferName(identifier: str) -> str:
			return "io_buffer_" + self.fqnToNameStr(identifier)

		args = []
		factoryTypes = {"sink": "makeSink()", "source": "makeSource()"}
		factoryStubTypes = {
		    "sink": "bzd::io::SinkStub",
		    "source": "bzd::io::SourceStub",
		}
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
				if kind == "sink":
					identifier = metadata["connections"][0]
					assert len(metadata["connections"]), f"Must be only one connection, {metadata['connections']}"
				else:
					identifier = metadata["identifier"]
				arg = f"{getBufferName(identifier)}.{factoryTypes[kind]}"

			args.append(f".{name} = {arg}")

		return ", ".join(args)

	# Generic iterable

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


def compositionCc(
    compositions: typing.Dict[str, CompositionView],
    output: Path,
    data: typing.Optional[Path] = None,
) -> None:
	template = Template.fromPath(Path(__file__).parent / "template/composition.cc.btl", indent=True)

	for target, composition in compositions.items():
		content = template.render(composition, Transform(composition=composition, data=data))
		(output.parent / f"{output.name}.{target}.cc").write_text(content)
