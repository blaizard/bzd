#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import re

definition_ = {
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

def getDefinition(kind):
	return definition_[kind] if kind in definition_ else definition_["default"]

class Member:
	def __init__(self, data):
		self.data = data

	def getDefinition(self):
		return getDefinition(self.data.get("kind", None))

	def isContainer(self):
		return self.getDefinition()["container"]

	def getName(self):
		return self.data.get("name", "")

	def getType(self):
		return self.data.get("type", "")

	def getKind(self):
		return self.data.get("kind", "")

	def getVirtual(self):
		return self.data.get("virtual", False)

	def getConst(self):
		return self.data.get("const", False)

	def getExplicit(self):
		return self.data.get("explicit", False)

	def getStatic(self):
		return self.data.get("static", False)

	def getTemplate(self):
		return self.data.get("template", [])

	def getArgs(self):
		return self.data.get("args", [])

	def getInheritance(self):
		return self.data.get("inheritance", [])

	def getDescriptionBrief(self):
		return self.data.get("descriptionBrief", "").strip()

	def getDescription(self):
		return self.data.get("description", "").strip()

	def printDefinition(self, formatStr = "{template} {pre} {type} {name} {post}"):

		def printTemplate(template):
			if len(template):
				formatArgs = ", ".join(["{type} {name}".format(type=v.get("type"), name=v.get("name", "")).strip() for v in template])
				return "template<{}>".format(formatArgs)
			return ""

		def printArgs(args):
			return ", ".join(["{type} {name}".format(type=v.get("type"), name=v.get("name", "")).strip() for v in args])

		formatRule = self.getDefinition()["format"].copy()
		for key, rule in formatRule.items():
			formatRule[key] = rule.format(
				name = self.getName(),
				type=self.getType(),
				virtual="virtual" if self.getVirtual() else "",
				const="const" if self.getConst() else "",
				explicit="explicit" if self.getExplicit() else "",
				static="static" if self.getStatic() else "",
				template=printTemplate(self.getTemplate()),
				args=printArgs(self.getArgs()))

		definition = formatStr.format(template=formatRule["template"],
				pre=formatRule["pre"],
				type=formatRule["type"],
				name=formatRule["name"],
				post=formatRule["post"])

		return re.sub(' +', ' ', definition.strip())

class MemberGroup:
	def __init__(self, memberList):
		self.list = []
		for member in memberList:
			self.list.append(Member(member))
		self.list = sorted(self.list, key=lambda k : k.getDefinition()["sort"])

	def get(self):
		return self.list

class Members:
	def __init__(self, data):
		self.data = {}
		for identifier, memberList in data.items():
			self.data[identifier] = MemberGroup(memberList)

		# Set metadata of specific containers
		for identifier, memberGroup in self.data.items():
			for member in memberGroup.get():
				if member.isContainer():
					print(identifier, member.getName(), member.getInheritance())

	def items(self):
		return self.data.items()
