#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import os
import re
import xml.etree.ElementTree as ET
import pprint
import glob
from render import Render

commonAttrs = {
	"id": {},
	"kind": {}, # "values": ["struct", "class", "variable", "function", "typedef", "dir", "file", "page", "namespace"] },
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
	"__type__": ["list"],
	"label": { "__content__": { "key": "name" } },
	"link": { "__attrs__": { "refid": { "key": "link" } } },
	"childnode": { "__attrs__": { "relation": { "key": "visibility", "values": { "public-inheritance": "public", "private-inheritance": "private", "protected-inheritance": "protected" } } } }
}

inheritancegraph = {
	"__type__": ["inheritance"],
	"node": node
}

memberdef = {
	"__type__": ["member"],
	"__attrs__": commonAttrs,
	"name": name,
	"type": { "__content__": { "key": "type" } },
	"templateparamlist": templateparamlist,
	"inheritancegraph": inheritancegraph,
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
}

dictionary = {
	"compounddef": compounddef
}

class DoxygenParser:

	def __init__(self):
		self.data = {}

	"""
	Write a warning message
	"""
	def assertTrue(self, condition, message, *argv):
		if not condition:
			print("[warning] %s" % (message.format(*argv)))
			raise Exception("Assert failed")

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
			return data["__info__"][key]
		return None

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
	Parse the attributes of the current element and return a dictionary
	"""
	def parseElement(self, element, definition, data = {}):
		if "__attrs__" in definition:
			for key, rules in definition["__attrs__"].items():
				# Get the value and add it only if it is non-None
				(key, value) = self.parseValue(rules, element.get(key, None), key)
				self.addMemberInfo(data, key, value)

		if "__content__" in definition:
			(key, value) = self.parseValue(definition["__content__"], "".join(element.itertext()))
			self.addMemberInfo(data, key, value)

	"""
	Check if a member has a namespace and if so, replace this member
	"""
	def normalizeName(self, data):
		self.assertTrue("__info__" in data, "Attribute '__info__' is missing from member: {}", data)
		self.assertTrue("name" in data["__info__"], "Attribute 'name' is missing from member: {}", data)
		namespaceList = data["__info__"]["name"].split("::")
		current = self.data
		for namespace in namespaceList[:-1]:
			current[namespace] = current[namespace] if namespace in current else {}
			current = current[namespace]
		data["__info__"]["name"] = namespaceList[-1]
		self.addMember(current, data)

	"""
	Generate an ID for a member relative to its namespace
	"""
	def makeMemberId(self, member):
		info = member["__info__"]
		formatStr = "{name}"
		if info["kind"] == "function":
			formatStr = "{template} {static} {virtual} {explicit} {type} {name}({args}) {const}"

		def printTemplate(template):
			formatArgs = ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in template])
			return "template<{}>".format(formatArgs)

		def printArgs(args):
			return ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in args])

		definition = formatStr.format(name = info["name"],
				type=info.get("type") if info.get("type", None) else "",
				virtual="virtual" if info.get("virtual", False) else "",
				const="const" if info.get("const", False) else "",
				explicit="explicit" if info.get("explicit", False) else "",
				static="static" if info.get("static", False) else "",
				template=printTemplate(info.get("template")) if info.get("template", False) else "",
				args=printArgs(info.get("args")) if info.get("args", False) else "")

		return re.sub(' +', ' ', definition.strip())

	"""
	Register a new member
	"""
	def addMember(self, data, member):
		identification = self.makeMemberId(member)
		data[identification] = data[identification] if identification in data else {}
		data[identification] = self.mergeMember(data[identification], member)

	"""
	Merge member2 into member1 and return it
	"""
	def mergeMember(self, member1, member2):

		self.assertTrue(type(member1) == type(member2), "Cannot merge different types {} vs {}", member1, member2)

		if isinstance(member2, dict):
			for key, value in member2.items():
				if key in member1:
					member1[key] = self.mergeMember(member1[key], value)
				else:
					member1[key] = value
		else:
			self.assertTrue(not isinstance(member2, list), "Member is a list")
			self.assertTrue(member1 == member2, "Trying to merge different members {} vs {}", member1, member2)

		return member1

	"""
	Parse a root XML element
	"""
	def parse(self, root, dictionary):
		self.parseSubElements(root, dictionary, {})

	def parseSubElements(self, root, dictionary, data):

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
							current = self.addMemberList(current, "inheritance")
						if "list" in definition["__type__"]:
							self.assertTrue(isinstance(current, list), "Data must be a list '{}'", current)
							current.append({})
							current = current[len(current) - 1]

					self.parseElement(element, definition, current)
					self.parseSubElements(element, definition, current)
					# Add this news element
					if "__type__" in definition and "member" in definition["__type__"]:
						self.addMember(data, current)

		# Check if there is a namespace
		if "__type__" in definition and "container" in definition["__type__"]:
			self.normalizeName(data)
		#print(self.current)

parser = DoxygenParser()

fileList = glob.glob("docs/xml/*Expected*")
for fileName in fileList:
	if fileName.lower().endswith(".xml"):
		root = ET.parse(fileName).getroot()
		parser.parse(root, dictionary)

#root = ET.parse('docs/xml/classbzd_1_1impl_1_1Expected.xml').getroot()
#parser.parse(root, dictionary)
#root = ET.parse('docs/xml/structbzd_1_1impl_1_1Expected_1_1RefWrapper.xml').getroot()
#parser.parse(root, dictionary)
#root = ET.parse('docs/xml/classbzd_1_1impl_1_1Expected_3_01void_00_01E_01_4.xml').getroot()
#parser.parse(root, dictionary)
#root = ET.parse('docs/xml/structbzd_1_1impl_1_1Expected_1_1ValueWrapper.xml').getroot()
#parser.parse(root, dictionary)

#root = ET.parse('docs/xml/namespacebzd.xml').getroot()
#parser.parse(root, dictionary)

pp = pprint.PrettyPrinter(indent=4)
pp.pprint(parser.data)

render = Render()
render.process(parser.data)
