#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import re

"""
Identifer:
Members are contained with identifiers. An identifier is the C++ namespace used to access the member.
Note, part of the identifier can be the name of a member, if this latter is a container. For example,
if Foo is a class present in the namespace bzd::impl, then members of Foo will have the identifer "bzd::impl::Foo"
TODO: Include template also in the identifier


"""

definition_ = {
	"struct": {
		"name": "Struct",
		"sort": 2,
		"container": True,
		"args": False,
		"defaultVisibility": "public",
		"format": {
			"template": "{template}",
			"pre": "",
			"post": "",
			"type": "struct",
			"name": "{name}"
		},
		"identifier": "{name}"
	},
	"class": {
		"name": "Class",
		"sort": 1,
		"container": True,
		"args": False,
		"defaultVisibility": "private",
		"format": {
			"template": "{template}",
			"pre": "",
			"post": "",
			"type": "class",
			"name": "{name}"
		},
		"identifier": "{name}"
	},
	"typedef": {
		"name": "Typedef",
		"sort": 4,
		"container": False,
		"args": False,
		"defaultVisibility": "private",
		"format": {
			"template": "{template}",
			"pre": "",
			"post": "",
			"type": "typedef",
			"name": "{name}"
		},
		"identifier": "{name}"
	},
	"variable": {
		"name": "Variable",
		"sort": 5,
		"container": False,
		"args": False,
		"defaultVisibility": "private",
		"format": {
			"template": "{template}",
			"pre": "{static}",
			"post": "",
			"type": "{const} {type}",
			"name": "{name}"
		},
		"identifier": "{name}"
	},
	"function": {
		"name": "Function",
		"sort": 3,
		"container": False,
		"args": True,
		"defaultVisibility": "private",
		"format": {
			"template": "{template}",
			"pre": "{static} {virtual} {explicit}",
			"post": "",
			"type": "{type}",
			"name": "{name}({args}) {const}"
		},
		"identifier": "{name}{template}"
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
		},
		"identifier": "{name}"
	}
}

def getDefinition(kind):
	return definition_[kind] if kind in definition_ else definition_["default"]

class Member:
	def __init__(self, data):
		self.data = data

	"""
	Generate a namespace identifier
	"""
	def makeIdentifier(self):
		return self.printDefinition(formatStr = self.getDefinition()["identifier"])

	def clone(self):
		return Member(self.data.copy())

	def setProvenance(self, provenance):
		self.data["provenance"] = provenance

	def getProvenance(self):
		return self.data.get("provenance", None)

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

	def getVisibility(self):
		return self.data.get("visibility", self.getDefinition()["defaultVisibility"])

	def getTemplate(self):
		return self.data.get("template", [])

	def getArgs(self):
		return self.data.get("args", [])

	def getInheritance(self):
		return self.data.get("inheritance", [])

	def getDescriptionBrief(self):
		regex = re.compile(r'[\n\r\t]')
		return regex.sub(" ", self.data.get("descriptionBrief", "").strip())

	def getDescription(self):
		return "{}\n{}".format(self.getDescriptionBrief(), self.data.get("description", "")).strip()

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
	def __init__(self, memberList, identifier):
		self.list = []
		self.identifier = identifier
		self.addMembers([Member(member) for member in memberList])
		self.parent = None

	def getParentMember(self):
		return self.parent

	"""
	Get constructor name if any
	"""
	def getConstructorName(self):
		return self.parent.getName() if self.parent else None

	"""
	Get destructor name if any
	"""
	def getDestructorName(self):
		return "~" + self.parent.getName() if self.parent else None

	def addMembers(self, members, provenance = None):
		name = self.getIdentifierName()
		for member in members:
			if member.getVisibility() == "public":
				if provenance:
					# If constructor or destructor, do not merge
					if self.parent and member.getName() in [self.getConstructorName(), self.getDestructorName()]:
						continue
					member.setProvenance(provenance)
				self.list.append(member)
		self.sort()

	def merge(self, memberGroup, provenance):
		self.addMembers([member.clone() for member in memberGroup.list], provenance)

	def sort(self):
		self.list = sorted(self.list, key=lambda k : k.getDefinition()["sort"])

	def get(self):
		return self.list

	def getMember(self, name):
		for member in self.list:
			if member.getName() == name:
				return member
		return None

	def getIdentifierName(self):
		name = self.identifier.split("::")[-1]
		return re.sub(r'<.*>', '', name).strip()

class Members:
	def __init__(self, data):
		self.data = {}
		for identifier, memberList in data.items():
			self.data[identifier] = MemberGroup(memberList, identifier)

		# Set metadata of specific containers
		for identifier, memberGroup in self.data.items():
			for member in memberGroup.get():
				if member.isContainer():
					containerMemberGroup = self.getMemberGroup("::".join([identifier, member.getName()]))
					# Set the parent member if any
					if containerMemberGroup:
						containerMemberGroup.parent = member

					for inheritance in member.getInheritance():
						if "id" in inheritance:
							#tempMemberGroup = self.getMemberGroup("::".join([identifier, member.getName()]), createIfNotExists=True)
							#print("Merge ", "::".join([identifier, member.getName()]), inheritance["id"])
							childMemberGroup = self.getMemberGroup(inheritance["id"])
							if containerMemberGroup and childMemberGroup:
								containerMemberGroup.merge(childMemberGroup, inheritance["id"])
							#print(memberGroup, self.getMember(inheritance["id"]))
							#print("::".join([identifier, member.getName()]))
				#print(identifier, member.getName())
				#exit()

			print(memberGroup.identifier, memberGroup.getIdentifierName())

	def getMemberGroup(self, identifier, createIfNotExists = False):
		if identifier in self.data:
			return self.data[identifier]
		return None

	def getMember(self, namespace):
		identifier = "::".join(namespace.split("::")[:-1])
		memberGroup = self.getMemberGroup(identifier)
		if memberGroup:
			name = namespace.split("::")[-1]
			return memberGroup.getMember(name)
		return None

	def items(self):
		return self.data.items()
