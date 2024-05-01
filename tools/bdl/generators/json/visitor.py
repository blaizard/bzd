import json
import typing
import ast
import pathlib

from tools.bdl.object import Object
from tools.bdl.visitors.composition.visitor import CompositionView
from tools.bdl.visitors.composition.components import ExpressionEntry
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved

Json = typing.Dict[str, typing.Any]


def formatJson(bdl: Object, data: typing.Optional[pathlib.Path] = None) -> str:
	return ""


def parametersToJson(parameters: ParametersResolved) -> Json:
	expressions = []
	for parameter in parameters:
		data = expressionToJson(parameter.expected) | expressionToJson(parameter.param)
		expressions.append(data)
	return expressions


def expressionToJson(expression: Expression) -> Json:
	data = {}
	if expression.isName:
		data["name"] = expression.name
	if expression.isSymbol:
		data["symbol"] = str(expression.symbol)
	if expression.isLiteral:
		data["value"] = expression.literalNative
	elif expression.isValue:
		data["value"] = str(expression.value)
	elif expression.isParameters:
		data["parameters"] = parametersToJson(expression.parametersResolved)
	return data


def expressionEntryToJson(entry: ExpressionEntry) -> Json:
	return {
	    "expression": expressionToJson(entry.expression),
	    "init": [expressionEntryToJson(expression) for expression in entry.init],
	    "intra": [expressionEntryToJson(expression) for expression in entry.intra],
	    "shutdown": [expressionEntryToJson(expression) for expression in entry.shutdown],
	    "deps": [expressionEntryToJson(expression) for expression in entry.deps],
	}


def compositionJson(
    compositions: typing.Dict[str, CompositionView],
    output: pathlib.Path,
    data: typing.Optional[pathlib.Path] = None,
) -> None:

	for target, composition in compositions.items():

		contexts = []

		for index, context in enumerate(composition.contexts):
			workloads = [expressionEntryToJson(workload) for workload in composition.workloads.get(context, [])]
			services = [expressionEntryToJson(service) for service in composition.services.get(context, [])]
			contexts.append({"workloads": workloads, "services": services})

		data = json.dumps({"contexts": contexts}, indent=4)
		(output.parent / f"{output.name}.{target}.json").write_text(data)
