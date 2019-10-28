#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import re

class Render:

	def __init__(self):
		pass

	def printMemberDefinition(self, member, formatStr = "{template} {pre} {type} {name} {post}"):

		formatRules = {
			"struct": {
				"template": "{template}",
				"pre": "",
				"post": "",
				"type": "struct",
				"name": "{name}"
			},
			"class": {
				"template": "{template}",
				"pre": "",
				"post": "",
				"type": "class",
				"name": "{name}"
			},
			"typedef": {
				"template": "{template}",
				"pre": "",
				"post": "",
				"type": "typedef",
				"name": "{name}"
			},
			"variable": {
				"template": "{template}",
				"pre": "{static}",
				"post": "",
				"type": "{const} {type}",
				"name": "{name}"
			},
			"function": {
				"template": "{template}",
				"pre": "{static} {virtual} {explicit}",
				"post": "",
				"type": "{type}",
				"name": "{name}({args}) {const}"
			},
			"default": {
				"template": "",
				"pre": "",
				"post": "",
				"type": "",
				"name": "{name}"
			}
		}

		def printTemplate(template):
			formatArgs = ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in template])
			return "template<{}>".format(formatArgs)

		def printArgs(args):
			return ", ".join(["{type} {name}".format(type=v["__info__"].get("type"), name=v["__info__"].get("name", "")).strip() for v in args])

		formatRule = formatRules[member["kind"]] if member["kind"] in formatRules else formatRules["default"]
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

	def createListing(self, namespace, members):
		sorted(members, key=lambda k: k.get("kind", None))
		prevKind = -1
		print("")
		print("")
		print("# {}".format(namespace))
		for member in members:
			kind = member.get("kind", None)
			if kind != prevKind:
				print("### {}".format(kind))
				print("|||")
				print("|---:|:---|")
			print("|{}|`{}`|".format(self.printMemberDefinition(member, "{type}"), self.printMemberDefinition(member, "{name}")))
			prevKind = kind

	def process(self, data):
		for namespace, members in data.items():
			print(namespace)
			self.createListing(namespace, members)
			for member in members:
				print(" - %s" % (self.printMemberDefinition(member)))
