#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import re
import os

class Render:

	def __init__(self, path):
		self.path = path
		self.curDir = None
		self.fileHandle = None

	def getDef(self, kind):
		definition = {
			"struct": {
				"name": "Struct",
				"sort": 2,
				"container": True,
				"args": False,
				"format": {
					"template": "{template}",
					"pre": "",
					"post": "",
					"type": "struct",
					"name": "{name}"
				}
			},
			"class": {
				"name": "Class",
				"sort": 1,
				"container": True,
				"args": False,
				"format": {
					"template": "{template}",
					"pre": "",
					"post": "",
					"type": "class",
					"name": "{name}"
				}
			},
			"typedef": {
				"name": "Typedef",
				"sort": 4,
				"container": False,
				"args": False,
				"format": {
					"template": "{template}",
					"pre": "",
					"post": "",
					"type": "typedef",
					"name": "{name}"
				}
			},
			"variable": {
				"name": "Variable",
				"sort": 5,
				"container": False,
				"args": False,
				"format": {
					"template": "{template}",
					"pre": "{static}",
					"post": "",
					"type": "{const} {type}",
					"name": "{name}"
				}
			},
			"function": {
				"name": "Function",
				"sort": 3,
				"container": False,
				"args": True,
				"format": {
					"template": "{template}",
					"pre": "{static} {virtual} {explicit}",
					"post": "",
					"type": "{type}",
					"name": "{name}({args}) {const}"
				}
			},
			"default": {
				"name": "",
				"sort": 6,
				"container": False,
				"args": False,
				"format": {
					"template": "",
					"pre": "",
					"post": "",
					"type": "",
					"name": "{name}"
				}
			}
		}
		return definition[kind] if kind in definition else definition["default"]

	def printMemberDefinition(self, member, formatStr = "{template} {pre} {type} {name} {post}"):

		def printTemplate(template):
			formatArgs = ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in template])
			return "template<{}>".format(formatArgs)

		def printArgs(args):
			return ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in args])

		formatRule = self.getDef(member["kind"])["format"]
		for key, rule in formatRule.items():
			formatRule[key] = rule.format(name = member["name"],
				type=member.get("type") if member.get("type", None) else "",
				virtual="virtual" if member.get("virtual", False) else "",
				const="const" if member.get("const", False) else "",
				explicit="explicit" if member.get("explicit", False) else "",
				static="static" if member.get("static", False) else "",
				template=printTemplate(member.get("template")) if member.get("template", False) else "",
				args=printArgs(member.get("args")) if member.get("args", False) else "")

		definition = formatStr.format(template=formatRule["template"],
				pre=formatRule["pre"],
				type=formatRule["type"],
				name=formatRule["name"],
				post=formatRule["post"])

		return re.sub(' +', ' ', definition.strip())

	def generateLink(self, namespace, member = None):
		namespaceList = namespace.split("::")
		path = os.path.join(self.path, os.path.join(*[self.toFileName(name) for name in namespaceList]))
		# Generate a raltive link
		if self.curDir:
			path = os.path.relpath(path, self.curDir)
		if member:
			if self.getDef(member.get("kind", None))["container"]:
				path = os.path.join(path, self.toFileName(member["name"]), "index.md")
		else:
			path = os.path.join(path, "index.md")
		return path

	def createListing(self, namespace, members):
		prevKind = -1
		self.fileHandle.write("# {}\n".format(namespace))
		for member in members:
			kind = member.get("kind", None)
			if kind != prevKind:
				self.fileHandle.write("### {}\n".format(self.getDef(kind)["name"]))
				self.fileHandle.write("|||\n")
				self.fileHandle.write("|---:|:---|\n")

			self.fileHandle.write("|{}|[`{}`]({})|\n".format(
					self.printMemberDefinition(member, "{type}"),
					self.printMemberDefinition(member, "{name}"),
					self.generateLink(namespace, member)))
			prevKind = kind

	def createMember(self, namespace, member):
		self.fileHandle.write("------\n")
		self.fileHandle.write("### `{} {}`\n".format(
				self.printMemberDefinition(member, "{template} {pre} {type}"),
				self.printMemberDefinition(member, "{name} {post}")))

		definition = self.getDef(member.get("kind", None))

		# Set the arguments
		if definition["args"] and member.get("args", None):
			self.fileHandle.write("#### Parameters\n")
			self.fileHandle.write("|||\n")
			self.fileHandle.write("|---:|:---|\n")
			for arg in member.get("args"):
				self.fileHandle.write("|{}|{}|\n".format(arg["__info__"].get("type"), arg["__info__"].get("name")))

	def toFileName(self, name):
		return re.sub(r'[^a-z0-9]+', '_', name.lower())

	def useFile(self, namespace):
		# Create directories
		path = self.generateLink(namespace)
		self.curDir = os.path.dirname(path)
		os.makedirs(self.curDir, exist_ok=True)
		self.fileHandle = open(path, "w")

	def closeFile(self):
		self.fileHandle.close()
		self.fileHandle = None
		self.curDir = None

	def process(self, data):
		for namespace, members in data.items():

			self.useFile(namespace)

			# Sort the members
			members = sorted(members, key=lambda k : self.getDef(k.get("kind", None))["sort"])

			self.createListing(namespace, members)
			for member in members:
				self.createMember(namespace, member)
				print(" - %s" % (self.printMemberDefinition(member)))

			self.closeFile()
