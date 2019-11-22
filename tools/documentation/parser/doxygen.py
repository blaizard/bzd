#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import os
import re
import xml.etree.ElementTree as ET
import pprint
import glob
import argparse

from members import Members, Member

kindToKeep = ["struct", "class", "variable", "function", "typedef", "namespace"]


def commentParser(element, data = {}):

	textList = []

	for para in element.findall("para"):
		parameterlistList = para.findall("parameterlist")
		programlistingList = para.findall("programlisting")
		if len(parameterlistList):
			for parameterlist in parameterlistList:
				kind = parameterlist.get("kind")
				paramList = []
				for parameteritem in parameterlist.findall("parameteritem"):
					param = {}
					if parameteritem.find("parameternamelist") is not None:
						param["name"] = ", ".join(text for text in parameteritem.find("parameternamelist").itertext() if text.strip())
					if parameteritem.find("parameterdescription") is not None:
						param["description"] = commentParser(parameteritem.find("parameterdescription"))
					paramList.append(param)
				data["__info__"] = data["__info__"] if "__info__" in data else {}
				data["__info__"]["description.%s" % (kind)] = paramList
		elif len(programlistingList):
			for programlisting in programlistingList:
				codeBlock = "<code>";
				for codeline in programlisting.findall("codeline"):
					codeBlock += " ".join(text for text in codeline.itertext() if text.strip()) + "\n"
				codeBlock += "</code>"
				textList.append(codeBlock)
		else:
			textList.append("".join(text for text in para.itertext() if text.strip()))

	return "\n".join(text for text in textList if text.strip())

commonAttrs = {
	"id": {},
	"kind": {"values": ["struct", "class", "variable", "function", "typedef", "namespace"], "others": "ignore"},
	"prot": { "values": ["private", "public", "protected"], "key": "visibility" },
	"static": { "values": {"no": False, "yes": True} },
	"mutable": { "values": {"no": False, "yes": True} },
	"const": { "values": {"no": False, "yes": True} },
	"explicit": { "values": {"no": False, "yes": True} },
	# Seems to be wrong most of the time
	#"inline": { "values": {"no": False, "yes": True} },
	"virt": { "key": "virtual", "values": {"non-virtual": False, "virtual": True, "pure-virtual": True} }
}

name = {
	"__content__": { "key": "name" }
}

param = {
	"__type__": ["list"],
	"type": { "__content__": { "key": "type" } },
	"declname": { "__content__": { "key": "name" } }
}

args = {
	"__type__": ["args", "list"],
	"type": { "__content__": { "key": "type" } },
	"declname": { "__content__": { "key": "name" } }
}

templateparamlist = {
	"__type__": ["template"],
	"param": param
}

node = {
	"__type__": ["dict"],
	"__attrs__": {"id": { "key": "__key__" }},
	"label": { "__content__": { "key": "name" } },
	"link": { "__attrs__": { "refid": { "key": "link" } } },
	"childnode": { 
		"__type__": ["children", "list"],
		"__attrs__": {
		"refid": { "key": "id" },
		"relation": { "key": "visibility", "values": { "public-inheritance": "public", "private-inheritance": "private", "protected-inheritance": "protected" }, "others": "ignore" } } }
}

briefdescription = {
	"__content__": { "key": "descriptionBrief" }
}

detaileddescription = {
	"__content__": { "key": "description", "parser": commentParser }
}

inheritancegraph = {
	"__type__": ["inheritance"],
	"node": node
}

memberdef = {
	"__type__": ["member"],
	"__attrs__": commonAttrs,
	"name": name,
	"type": {
		"__content__": { "key": "type" },
		"ref": {
			"__type__": ["typeref", "list"],
			"__attrs__": {
				"refid": { "key": "link" }
			},
		}
	},
	"templateparamlist": templateparamlist,
	"inheritancegraph": inheritancegraph,
	"briefdescription": briefdescription,
	"detaileddescription": detaileddescription,
	"param": args
}

sectiondef = {
	"memberdef": memberdef
}

compoundname = {
	"__content__": { "key": "name" }
}

compounddef = {
	"__type__": ["container"],
	"__attrs__": commonAttrs,
	"compoundname": compoundname,
	"sectiondef": sectiondef,
	"templateparamlist": templateparamlist,
	"inheritancegraph": inheritancegraph,
	"briefdescription": briefdescription,
	"detaileddescription": detaileddescription
}

dictionaryRoot = {
	"compounddef": compounddef
}

class DoxygenParser:

	def __init__(self):
		self.data = {}
		self.groups = {}

	"""
	Write a warning message
	"""
	def assertTrue(self, condition, message, *argv):
		if not condition:
			print("[error] %s" % (message.format(*argv)))
			raise Exception("Assert failed")

	"""
	Write a warning message
	"""
	def assertTrueWarning(self, condition, message, *argv):
		if not condition:
			print("[warning] %s" % (message.format(*argv)))
			return True
		return False

	"""
	Parse a value and returns a key value pair
	"""
	def parseValue(self, rules, value, key = None):
		if value and "values" in rules:
			acceptedValues = []
			if isinstance(rules["values"], list):
				acceptedValues = [v for v in rules["values"] if v == value.lower()]
			elif isinstance(rules["values"], dict):
				acceptedValues = [v for k, v in rules["values"].items() if k == value.lower()]
			if len(acceptedValues) == 1:
				value = acceptedValues[0]
			else:
				self.assertTrue(True if "others" in rules and rules["others"] == "ignore" else False, "Value '{}' not accepted for key '{}'", value, key)
				return (key, None)

		if "key" in rules:
			key = rules["key"]
		self.assertTrue(key != None, "The 'key' attribute for rule '{}' is missing", rules)
		return (key, value)

	"""
	Add member information
	"""
	def addMemberInfo(self, data, key, value):
		if value != None:
			data["__info__"] = data["__info__"] if "__info__" in data else {}
			if key in data["__info__"]:
				self.assertTrue(data["__info__"][key] == value, "Overriding key '{}' with a different value '{}' vs '{}': {}", key, data["__info__"][key], value, data)
			data["__info__"][key] = value

	"""
	Add member list
	"""
	def addMemberList(self, data, key):
		data["__info__"] = data["__info__"] if "__info__" in data else {}
		if key not in data["__info__"]:
			data["__info__"][key] = []
		self.assertTrue(isinstance(data["__info__"][key], list), "The key '{}' must be a list", key)
		return data["__info__"][key]

	"""
	Add member dictionary
	"""
	def addMemberDict(self, data, key):
		data["__info__"] = data["__info__"] if "__info__" in data else {}
		if key not in data["__info__"]:
			data["__info__"][key] = {}
		self.assertTrue(isinstance(data["__info__"][key], dict), "The key '{}' must be a dictionary", key)
		return data["__info__"][key]

	"""
	Parse the attributes of the current element and return a dictionary
	"""
	def parseElement(self, element, definition, data = {}):
		if "__attrs__" in definition:
			for key, rules in definition["__attrs__"].items():
				# Get the value and add it only if it is non-None
				(key, value) = self.parseValue(rules, element.get(key, None), key)
				self.addMemberInfo(data, key, value)

		if "__content__" in definition:
			if definition["__content__"].get("parser", False):
				value = definition["__content__"].get("parser")(element, data)
			else:
				value = "".join(element.itertext())
			(key, value) = self.parseValue(definition["__content__"], value)
			self.addMemberInfo(data, key, value)

	"""
	Check if a member has a namespace and if so, replace this member
	"""
	def normalizeName(self, data):
		if self.assertTrueWarning("__info__" in data, "Attribute '__info__' is missing from member: {}", data):
			return
		self.assertTrue("name" in data["__info__"], "Attribute 'name' is missing from member: {}", data)
		namespaceList = data["__info__"]["name"].split("::")
		current = self.data
		for namespace in namespaceList[:-1]:
			current[namespace] = current[namespace] if namespace in current else {}
			current = current[namespace]
		data["__info__"]["name"] = namespaceList[-1]
		self.addMember(current, data)

	"""
	Register a new member
	"""
	def addMember(self, data, member):
		if member["__info__"].get("kind", None) in kindToKeep:

			# Generate the member identifier
			memberData = self.removeNestedKeyword(member["__info__"], "__info__")
			identification = Member(memberData).makeIdentifier()

			data[identification] = data[identification] if identification in data else {}
			data[identification] = self.mergeMember(data[identification], member)

	"""
	Merge member2 into member1 and return it
	"""
	def mergeMember(self, member1, member2):
		self.assertTrue(type(member1) == type(member2), "Cannot merge different types {} vs {}", member1, member2)

		# Merge dictionaries
		if isinstance(member2, dict):
			for key, value in member2.items():
				if key in member1:
					member1[key] = self.mergeMember(member1[key], value)
				else:
					member1[key] = value

		# Merge 2 list
		elif isinstance(member2, list):
			for i in range(len(member2)):
				if len(member1) > i:
					member1[i] = self.mergeMember(member1[i], member2[i])
				else:
					member1.append(member2[i])

		else:
			self.assertTrue(member1 == member2, "Trying to merge different members {} vs {}", member1, member2)

		return member1

	"""
	Remove nested keyword
	"""
	def removeNestedKeyword(self, definition, keyword):
		if isinstance(definition, dict):
			if keyword in definition:
				self.assertTrue(isinstance(definition[keyword], dict), "Sub definition under '{}' must be a dict", keyword)
				return self.removeNestedKeyword(definition[keyword], keyword)
			for key, value in definition.items():
				definition[key] = self.removeNestedKeyword(value, keyword)
		elif isinstance(definition, list):
			for i in range(len(definition)):
				definition[i] = self.removeNestedKeyword(definition[i], keyword)
		return definition

	"""
	Format the data into groups
	"""
	def makeGroups(self, groups, data, namespaceList = []):

		for namespace, definition in data.items():
			if namespace == "__info__":
				namespaceStr = "::".join(namespaceList[:-1])
				groups[namespaceStr] = groups[namespaceStr] if namespaceStr in groups else []
				# Remove nested __info__
				definition = self.removeNestedKeyword(definition, "__info__")
				groups[namespaceStr].append(definition)
			else:
				namespaceList.append(namespace)
				self.makeGroups(groups, definition, namespaceList)
				namespaceList.pop()

	"""
	Resolve and set links
	"""
	def resolveLinks(self, groups):
		links = {}

		def addLink(dataList):
			for data in dataList:
				link = data.get("link", None)
				if link:
					links[link] = links[link] if link in links else []
					links[link].append(data)
					del data["link"]

		# Collect the list of IDs that must be set together
		for namespace, members in groups.items():
			for member in members:
				if "inheritance" in member:

					# Default visibility is public
					inheritanceCurrent = None
					for linkId, parent in member["inheritance"].items():
						if "link" in parent:
							if member.get("id", None) == parent["link"]:
								inheritanceCurrent = parent

					# Build the inheritance chain
					def	getInheritance(inheritanceCurrent, inheritanceList, visibility = "public"):
						if inheritanceCurrent:
							for child in inheritanceCurrent.get("children", []):
								inheritanceNext = child
								visibility = inheritanceNext.get("visibility", visibility)
								linkId = inheritanceNext.get("id", None)
								self.assertTrue(linkId != None, "Missing link ID: {}", inheritanceCurrent)
								inheritanceNext = member["inheritance"].get(linkId, None)
								if inheritanceNext:
									inheritanceList.append({ "visibility": visibility, "link": inheritanceNext.get("link", None), "id": inheritanceNext.get("name", None) })
									getInheritance(inheritanceNext, inheritanceList, visibility)
					inheritanceList = []
					getInheritance(inheritanceCurrent, inheritanceList)
					addLink(inheritanceList)
					member["inheritance"] = inheritanceList

				addLink(member.get("typeref", []))

		# Resolve links
		for namespace, members in groups.items():
			for member in members:
				if "id" in member:
					link = member["id"]
					del member["id"]
					if link in links:
						linkId = "::".join([namespace, member["name"]])
						for target in links[link]:
							target["id"] = linkId

	"""
	Merge description with their params and template params
	"""
	def mergeDescription(self, groups):
		for namespace, members in groups.items():
			for member in members:
				if "description.param" in member:
					self.mergeMember(member, {"args": member["description.param"]})
					del member["description.param"]
				if "description.templateparam" in member:
					self.mergeMember(member, {"template": member["description.templateparam"]})
					del member["description.templateparam"]

	"""
	Parse a root XML element
	"""
	def parse(self, root, dictionary = dictionaryRoot):
		self.parseSubElements(root, dictionary, {})

	"""
	Build the database and group data together
	"""
	def getMembersData(self):
		groups = {}
		self.makeGroups(groups, self.data)
		self.mergeDescription(groups)
		self.resolveLinks(groups)
		return groups

	def parseSubElements(self, root, dictionary, data):

		try:

			for name, definition in dictionary.items():
				if name not in ["__attrs__", "__content__", "__type__"]:
					for element in root.findall(name):
						current = data
						# Add new element
						if "__type__" in definition:
							if "member" in definition["__type__"]:
								current = {}
							if "template" in definition["__type__"]:
								current = self.addMemberList(current, "template")
							if "args" in definition["__type__"]:
								current = self.addMemberList(current, "args")
							if "inheritance" in definition["__type__"]:
								current = self.addMemberDict(current, "inheritance")
							if "children" in definition["__type__"]:
								current = self.addMemberList(current, "children")
							if "typeref" in definition["__type__"]:
								current = self.addMemberList(current, "typeref")
							if "list" in definition["__type__"]:
								self.assertTrue(isinstance(current, list), "Data must be a list '{}'", current)
								current.append({})
								current = current[len(current) - 1]
							if "dict" in definition["__type__"]:
								self.assertTrue(isinstance(current, dict), "Data must be a dictionary '{}'", current)

						self.parseElement(element, definition, current)
						self.parseSubElements(element, definition, current)

						# Add this news element
						if "__type__" in definition and "member" in definition["__type__"]:
							self.addMember(data, current)

						elif "__type__" in definition and "dict" in definition["__type__"]:
							self.assertTrue("__key__" in current["__info__"], "Data must have a key __key__: {}", current)
							current[current["__info__"]["__key__"]] = current["__info__"]
							del current["__info__"]["__key__"]
							del current["__info__"]

			# Check if there is a namespace
			if "__type__" in definition and "container" in definition["__type__"]:
				self.normalizeName(data)

		except Exception as e:
			print("current data: ", data, root)
			raise e
