import typing
import pathlib
import enum
import json
import dataclasses
import shutil

from tools.docker_images.ebook.providers import Provider, providerSerialize, providerDeserialize

FlowSchemaType = typing.List[str]


class FlowEnum(enum.Enum):
	pass


class ActionInterface(typing.Protocol):
	ProviderInput: typing.Any = None
	ProviderOutput: typing.Any = None

	def __init__(self) -> None:
		assert self.ProviderInput is not None, f"The ProviderInput must be set."
		assert self.ProviderOutput is not None, f"The ProviderOutput must be set."

	def process(self, input: ProviderInput,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderOutput, typing.Optional[FlowEnum]]]:
		raise NotImplementedError()


@dataclasses.dataclass
class FlowState:
	"""Describte the status of a the flow for a single document."""

	provider: Provider
	flow: FlowSchemaType
	index: int = 0

	def completed(self) -> bool:
		return len(self.flow) == 0

	def toJson(self) -> typing.Dict[str, typing.Any]:
		"""Serialize the current state."""
		return {"flow": self.flow, "provider": providerSerialize(self.provider), "index": self.index}

	@staticmethod
	def fromJson(data: typing.Dict[str, typing.Any]) -> "FlowState":
		return FlowState(provider=providerDeserialize(data["provider"]), flow=data["flow"], index=data["index"])


class FlowRegistry:

	def __init__(self, schema: typing.Type[FlowEnum], actions: typing.Dict[str, ActionInterface]) -> None:
		self.schema = schema
		self.actions = actions

		# Check that all actions are mapped.
		for flow in self.schema:
			for actionName in flow.value:
				assert actionName in self.actions, f"The action named '{actionName}' does not exists."

	def restoreOrReset(self, directory: pathlib.Path, flowKey: FlowEnum, provider: Provider) -> "Flow":
		"""Restore a flow if a previous state exists, other starts from scratch."""

		if Flow.stateJson(directory).exists():
			return self.restore(directory)
		return self.reset(directory, flowKey, provider)

	def reset(self, directory: pathlib.Path, flowKey: FlowEnum, provider: Provider) -> "Flow":
		"""Start a flow from the start."""

		state = FlowState(provider=provider, flow=flowKey.value)
		return Flow(self, directory, [state])

	def restore(self, directory: pathlib.Path) -> "Flow":
		"""Restore a flow from a previous position."""

		stateJson = Flow.stateJson(directory)
		assert stateJson.exists(), f"'{stateJson}' does not exists."

		data = json.loads(stateJson.read_text())
		states = [FlowState.fromJson(state) for state in data["states"]]
		return Flow(self, directory, states)

	def get(self, name: str) -> ActionInterface:
		"""Get a flow interface by name."""
		assert name in self.actions, f"The action named '{name}' does not exists."
		return self.actions[name]


class Flow:

	def __init__(self, registry: FlowRegistry, directory: pathlib.Path, states: typing.List[FlowState]) -> None:
		self.registry = registry
		self.directory = directory
		self.states = states

	@staticmethod
	def stateJson(directory: pathlib.Path) -> pathlib.Path:
		return directory / "state.json"

	def run(self) -> typing.List[Provider]:

		while any(not state.completed() for state in self.states):

			# Serialize the current state.
			data = {"states": [state.toJson() for state in self.states]}
			Flow.stateJson(self.directory).write_text(json.dumps(data, indent=4))

			# Get the next non completed document state.
			for documentIndex, state in enumerate(self.states):
				if not state.completed():
					break

			actionName = state.flow.pop(0)
			action = self.registry.get(actionName)
			workingDirectory = self.directory / f"{(documentIndex + 1):02}.{(state.index + 1):02}.{actionName}"
			if workingDirectory.exists():
				shutil.rmtree(workingDirectory)
			workingDirectory.mkdir(parents=True, exist_ok=True)
			print(f"--- {actionName} ({workingDirectory})")

			outputs = action.process(state.provider, workingDirectory)
			isFirst = True
			currentFlow = state.flow.copy()
			for provider, maybeFlow in outputs:
				flow = ([] if maybeFlow is None else maybeFlow.value) + currentFlow
				if isFirst:
					state.provider = provider
					state.flow = flow
					state.index += 1
					isFirst = False
				else:
					self.states.append(FlowState(provider=provider, flow=flow))

		return [state.provider for state in self.states]
