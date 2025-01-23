import json
import typing
import ast
import pathlib

from bdl.object import Object
from bdl.visitors.composition.visitor import CompositionView
from bdl.visitors.composition.components import ExpressionEntry
from bdl.entities.impl.expression import Expression
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolved

Json = typing.Dict[str, typing.Any]


def formatJson(bdl: Object, data: typing.Optional[pathlib.Path] = None) -> str:
	return ""


def parametersToJson(parameters: ParametersResolved) -> typing.Sequence[Json]:
	expressions = []
	for parameter in parameters:
		data = expressionToJson(typing.cast(Expression, parameter.expected)) | expressionToJson(
		    typing.cast(Expression, parameter.param))
		expressions.append(data)
	return expressions


def expressionToJson(expression: Expression) -> Json:
	data: Json = {}
	if expression.isName:
		data["name"] = expression.name
	if expression.isName and expression.isFQN:
		data["fqn"] = expression.fqn
	if expression.isSymbol:
		data["symbol"] = str(expression.symbol)
		data["category"] = str(expression.symbol.category.value)
	if expression.isLiteral:
		data["value"] = expression.literalNative
	elif expression.isValue:
		data["value"] = str(expression.value)
	elif expression.isParameters:
		data["parameters"] = parametersToJson(expression.parametersResolved)
	return data


def expressionToFQN(expression: Expression) -> str:
	assert expression.isFQN, f"Expression '{expression}' must have a FQN."
	assert expression.isName, f"Expression '{expression}' must have a name."
	return expression.fqn


def expressionEntryToJson(entry: ExpressionEntry) -> Json:
	return {
	    "expression": expressionToJson(entry.expression),
	    "init": [expressionToJson(expression) for expression in entry.init],
	    "intra": [expressionToJson(expression) for expression in entry.intra],
	    "shutdown": [expressionToJson(expression) for expression in entry.shutdown],
	    "deps": [expressionToFQN(expression) for expression in entry.deps],
	}


def compositionJson(
    compositions: typing.Dict[str, CompositionView],
    output: pathlib.Path,
    data: typing.Optional[pathlib.Path] = None,
) -> None:

	for target, composition in compositions.items():

		contexts = []

		for index, context in enumerate(composition.contexts):
			registry = {
			    uid: expressionEntryToJson(registry)
			    for uid, registry in composition.registry.get(context, {}).items()
			}
			workloads = [expressionEntryToJson(workload) for workload in composition.workloads.get(context, [])]
			services = [expressionEntryToJson(service) for service in composition.services.get(context, [])]
			contexts.append({"registry": registry, "workloads": workloads, "services": services})

		jsonData = json.dumps({"contexts": contexts}, indent=4)
		(output.parent / f"{output.name}.{target}.json").write_text(jsonData)
