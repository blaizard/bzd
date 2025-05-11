import typing
import pathlib
import enum
import json
import dataclasses

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
	            directory: pathlib.Path) -> typing.Tuple[ProviderOutput, typing.Optional[FlowEnum]]:
		raise NotImplementedError()


@dataclasses.dataclass
class FlowState:
	"""Describte the status of a the flow for a single document."""

	provider: Provider
	flow: FlowSchemaType
	index: int

	def serialize(self) -> str:
		"""Serialize the current state."""
		data = {"flow": self.flow, "provider": providerSerialize(self.provider), "index": self.index}
		return json.dumps(data)

	@staticmethod
	def deserialize(serialized) -> "FlowState":
		data = json.loads(serialized)
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

		state = FlowState(provider=provider, flow=flowKey.value, index=0)
		return Flow(self, directory, state)

	def restore(self, directory: pathlib.Path) -> "Flow":
		"""Restore a flow from a previous position."""

		stateJson = Flow.stateJson(directory)
		assert stateJson.exists(), f"'{stateJson}' does not exists."

		state = FlowState.deserialize(stateJson.read_text())
		return Flow(self, directory, state)

	def get(self, name: str) -> ActionInterface:
		"""Get a flow interface by name."""
		assert name in self.actions, f"The action named '{name}' does not exists."
		return self.actions[name]


class Flow:

	def __init__(self, registry: FlowRegistry, directory: pathlib.Path, state: FlowState) -> None:
		self.registry = registry
		self.directory = directory
		self.state = state

	@staticmethod
	def stateJson(directory: pathlib.Path) -> pathlib.Path:
		return directory / "state.json"

	def run(self) -> None:

		while self.state.flow:
			# Serialize the current state.
			Flow.stateJson(self.directory).write_text(self.state.serialize())

			actionName = self.state.flow.pop(0)
			action = self.registry.get(actionName)
			workingDirectory = self.directory / f"{(self.state.index + 1):02}.{actionName}"
			workingDirectory.mkdir(parents=True, exist_ok=True)
			print(f"--- {actionName} ({workingDirectory})")

			self.state.provider, maybeFlow = action.process(self.state.provider, workingDirectory)
			if maybeFlow is not None:
				self.state.flow = maybeFlow.value + self.state.flow
			self.state.index += 1
