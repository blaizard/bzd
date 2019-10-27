import re

class Render:

	def __init__(self):
		pass

	def printMemberDefinition(self, member):
		formatStr = "{name}"
		if member["kind"] == "struct":
			formatStr = "{template} struct {name}"
		elif member["kind"] == "class":
			formatStr = "{template} class {name}"
		elif member["kind"] == "typedef":
			formatStr = "{template} typedef {name}"
		elif member["kind"] == "variable":
			formatStr = "{template} {static} {const} {type} {name}"
		elif member["kind"] == "function":
			formatStr = "{template} {static} {virtual} {explicit} {type} {name}({args}) {const}"

		def printTemplate(template):
			formatArgs = ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in template])
			return "template<{}>".format(formatArgs)

		def printArgs(args):
			return ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in args])

		definition = formatStr.format(name = member["name"],
				type=member.get("type") if member.get("type", None) else "",
				virtual="virtual" if member.get("virtual", False) else "",
				const="const" if member.get("const", False) else "",
				explicit="explicit" if member.get("explicit", False) else "",
				static="static" if member.get("static", False) else "",
				template=printTemplate(member.get("template")) if member.get("template", False) else "",
				args=printArgs(member.get("args")) if member.get("args", False) else "")

		return re.sub(' +', ' ', definition.strip())

	def processMembers(self, members, namespaceList):
		print("processMembers", namespaceList)

		"::".join(namespaceList)
		for identifier, member in members.items():
			print(self.printMemberDefinition(member))
			if "__members__" in member:
				namespaceList.append(member["name"])
				self.processMembers(member["__members__"], namespaceList)
				namespaceList.pop()

	def processNamespace(self, data, namespaceList):
		for namespace, definition in data.items():
			if namespace == "__members__":
				self.processMembers(definition, namespaceList)
			else:
				namespaceList.append(namespace)
				self.processNamespace(definition, namespaceList)
				namespaceList.pop()

	def process(self, data):
		self.processNamespace(data, [])
