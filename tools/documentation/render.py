#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import re

class Render:

	def __init__(self):
		self.groups = {}

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

	def processMembers(self, member, namespaceList):
		namespace = "::".join(namespaceList[:-1])
		self.groups[namespace] = self.groups[namespace] if namespace in self.groups else []
		self.groups[namespace].append(member)

	def makeGroups(self, data, namespaceList = []):
		for namespace, definition in data.items():
			if namespace == "__info__":
				self.processMembers(definition, namespaceList)
			else:
				namespaceList.append(namespace)
				self.makeGroups(definition, namespaceList)
				namespaceList.pop()

	def process(self, data):
		self.makeGroups(data)

		for namespace, members in self.groups.items():
			print(namespace)
			for member in members:
				print(" - %s" % (self.printMemberDefinition(member)))

