from typing import Optional, Mapping, Any, Set, Sequence, Iterable, List, Dict, MutableMapping, ValuesView

from tools.bzd.generator.manifest.object import Object
from tools.bzd.generator.manifest.interface import Interface, EmptyInterface
from tools.bzd.generator.manifest.artifact import Artifact
from tools.bzd.generator.manifest.validator import Validator
"""
Represent a key, used together with the context for messaging
"""


class _Keys():

	def __init__(self) -> None:
		self.keys: List[str] = []

	def push(self, key: str) -> None:
		self.keys.append(key)

	def pop(self) -> None:
		self.keys.pop()

	def toKey(self) -> str:
		return "/".join(self.keys)

	def __str__(self) -> str:
		return " > ".join(self.keys)


class Manifest():

	def __init__(self) -> None:
		self.data: Dict[str, Any] = {}
		self.objects: Dict[str, Object] = {}
		self.interfaces: Dict[str, Interface] = {}
		self.artifacts: Dict[str, Artifact] = {}
		self.setRenderer()
		self.format = {
			"artifacts": {
			"_default": {
			"_key": Validator("any"),
			"path": Validator("path")
			}
			},
			"interfaces": {
			"_default": {
			"_key": Validator("interface"),
			"includes": Validator("any"),
			"implementation": Validator("interface")
			}
			},
			"objects": {
			"_default": {
			"_key": Validator("object"),
			"implementation": Validator("interface"),
			"params": Validator("any"),
			"config": {
			"_key": Validator("any"),
			"_default": Validator("any")
			}
			}
			}
		}

	"""
	Set a new renderer for reference objects
	"""

	def setRenderer(self, renderer: Optional[Mapping[str, Any]] = None) -> None:
		self.renderer = renderer

	"""
	List all objects
	"""

	def getObjects(self, filt: Optional[Mapping[str, Any]] = None) -> Iterable[Object]:
		config = {"interface": None}
		if filt is not None:
			config.update(filt)
		for identifier, obj in self.objects.items():
			if config["interface"] == None or config["interface"] == obj.getInterfaceName():
				yield obj

	"""
	Return true if this is a known interface, false otherwise
	"""

	def isInterface(self, name: str) -> bool:
		return str(name) in self.interfaces

	"""
	Get a specific inteface
	"""

	def getInterface(self, name: str, mustExists: bool = True) -> Interface:
		exists = self.isInterface(name)
		assert not mustExists or exists, "The interface name '{}' was not discovered or is not valid.".format(str(name))
		return self.interfaces[str(name)] if exists else EmptyInterface(name)

	"""
	Return the raw data of this manifest
	"""

	def getData(self) -> MutableMapping[str, Any]:
		return self.data

	"""
	Return the list of artifact objects
	"""

	def getArtifacts(self) -> ValuesView[Artifact]:
		return self.artifacts.values()

	"""
	List all depending interfaces
	"""

	def getDependentInterfaces(self) -> Set[Interface]:

		# Gather all object dependencies
		dependencies = set()
		for obj in self.getObjects():
			dependencies.update(obj.getDependentInterfaces())

		return dependencies

	"""
	Return the registry sorted by dependency resolution
	"""

	def getRegistry(self) -> List[Mapping[str, Any]]:
		registryEntries = {}
		interfaces = set([obj.getInterface() for obj in self.getObjects()])
		for interface in interfaces:
			objects = list(self.getObjects({"interface": interface.getName()}))
			# Generate dictionary of depending objects
			deps = {}
			[deps.update(obj.getDependentObjects()) for obj in objects]
			registryEntries[interface.getName()] = {"objects": objects, "deps": deps}
		# Create a dependency graph
		dependencyGraph: Mapping[str, Set[str]] = {interface: set() for interface in registryEntries.keys()}
		for interfaceStr, data in registryEntries.items():
			assert isinstance(data["deps"], dict)
			for dependency in data["deps"].keys():
				dependencyGraph[interfaceStr].add(dependency)

		# Sort the registry
		# Todo, can be optimized/simplified
		registryList: List[Mapping[str, Any]] = []
		while dependencyGraph:
			newDependencyGraph = {}
			registryListInitialLen = len(registryList)
			for interfaceStr, dependencies in dependencyGraph.items():
				if all([dep not in dependencyGraph for dep in dependencies]):
					registryList.append({
						"interface": interfaceStr,
						"objects": registryEntries[interfaceStr]["objects"]
					})
				else:
					newDependencyGraph[interfaceStr] = dependencies
			if registryListInitialLen == len(registryList):
				raise Exception("Circular depedency detected between interfaces: {}.".format(", ".join(
					dependencyGraph.keys())))
			dependencyGraph = newDependencyGraph

		return registryList

	"""
	Merge data into the current manifest.
	"""

	def merge(self, data: Mapping[str, Any], path: str) -> None:
		context = {"key": _Keys()}
		try:
			self._mergeAndValidate(self.data, data, self.format, context, path)
		except Exception as e:
			raise Exception("Error while merging ({}): {}".format(
				"; ".join(["{}: '{}'".format(str(key), str(text)) for key, text in context.items() if text]), e))

	"""
	Add artifacts to the generated code.
	"""

	def addArtifact(self, path: str, identifier: str) -> None:
		if "artifacts" not in self.data:
			self.data["artifacts"] = {}
		assert identifier not in self.data["artifacts"], "Identifier '{}' is already used for artifact.".format(
			identifier)
		self.data["artifacts"][identifier] = {"path": path}

	"""
	Process the current data of the manifest in order to speed-up later accesses
	"""

	def process(self) -> None:

		# Objects
		self.objects = {}
		for identifier in self.data.get("objects", {}).keys():
			try:
				self.objects[identifier] = Object(self, identifier)
			except Exception as e:
				raise Exception("Error while processing object {}: {}".format(identifier, e))

		# Interfaces
		self.interfaces = {}
		for identifier in self.data.get("interfaces", {}).keys():
			try:
				self.interfaces[identifier] = Interface(self, identifier)
			except Exception as e:
				raise Exception("Error while processing interface {}: {}".format(identifier, e))

		# Artifacts
		self.artifacts = {}
		for identifier in self.data.get("artifacts", {}).keys():
			try:
				self.artifacts[identifier] = Artifact(self, identifier)
				self.artifacts[identifier].registerObject(self.objects)
			except Exception as e:
				raise Exception("Error while processing artifact {}: {}".format(identifier, e))

	"""
	Merge the data
	"""

	@staticmethod
	def _mergeAndValidate(dst: MutableMapping[str, Any], src: Mapping[str, Any], validation: Mapping[str, Any],
		context: MutableMapping[str, Any], path: str) -> None:

		for key, value in src.items():
			context["key"].push(key)

			# Look for the validation data associated with this key
			vData = validation[key] if key in validation else (
				validation["_default"] if "_default" in validation else None)
			assert vData != None, "Invalid key '{}'.".format(key)

			# Validate the data
			if isinstance(vData, dict):
				assert isinstance(value, dict), "Key '{}' must contain a dictionary type object.".format(key)

				# If it contains key format validator
				if "_key" in vData:
					vData["_key"].validate(key)

				if key not in dst:
					dst[key] = {}

				# Go further
				Manifest._mergeAndValidate(dst[key], value, vData, context, path)

			else:
				vData.validate(value)
				if key not in dst:
					dst[key] = value
				else:
					assert dst[key] == value, "Conflicting values: '{}' != '{}'".format(str(dst[key]), str(value))

			context["key"].pop()
