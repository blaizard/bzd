import typing
import pathlib
import enum
import json

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

		return Flow(self, flowKey.value, directory, provider, 0)

	def restore(self, directory: pathlib.Path) -> "Flow":
		"""Restore a flow from a previous position."""

		stateJson = Flow.stateJson(directory)
		assert stateJson.exists(), f"'{stateJson}' does not exists."

		state = json.loads(stateJson.read_text())
		flow = state["flow"]
		index = state["index"]
		provider = providerDeserialize(state["provider"])
		return Flow(self, flow, directory, provider, index)

	def get(self, name: str) -> ActionInterface:
		"""Get a flow interface by name."""
		assert name in self.actions, f"The action named '{name}' does not exists."
		return self.actions[name]


class Flow:

	def __init__(self, registry: FlowRegistry, flow: FlowSchemaType, directory: pathlib.Path, provider: Provider,
	             index: int) -> None:
		self.registry = registry
		self.flow = flow
		self.directory = directory
		self.provider = provider
		self.index = index

	@staticmethod
	def stateJson(directory: pathlib.Path) -> pathlib.Path:
		return directory / "state.json"

	def run(self) -> None:

		while self.flow:
			# Serialize the current state.
			state = {"flow": self.flow, "provider": providerSerialize(self.provider), "index": self.index}
			Flow.stateJson(self.directory).write_text(json.dumps(state))

			actionName = self.flow.pop(0)
			action = self.registry.get(actionName)
			workingDirectory = self.directory / f"{(self.index + 1):02}.{actionName}"
			workingDirectory.mkdir(parents=True, exist_ok=True)
			print(f"--- {actionName} ({workingDirectory})")

			self.provider, maybeFlow = action.process(self.provider, workingDirectory)
			if maybeFlow is not None:
				self.flow = maybeFlow.value + self.flow
			self.index += 1
