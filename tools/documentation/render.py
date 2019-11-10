#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import re
import os

class Render:

	def __init__(self, path):
		self.path = path
		self.curDir = None
		self.fileHandle = None

	def generateLink(self, namespace, member = None):
		namespaceList = namespace.split("::")
		path = os.path.join(self.path, os.path.join(*[self.toFileName(name) for name in namespaceList]))
		# Generate a raltive link
		if self.curDir:
			path = os.path.relpath(path, self.curDir)
		if member:
			if member.isContainer():
				path = os.path.join(path, self.toFileName(member.getName()), "index.md")
		else:
			path = os.path.join(path, "index.md")
		return path

	def createNamespace(self, namespace, members):
		if namespace:
			self.fileHandle.write("# ")
			namespaceList = namespace.split("::")
			for i in range(len(namespaceList)):
				self.fileHandle.write("{}[`{}`]({})".format("::" if i else "", namespaceList[i], self.generateLink("::".join(namespaceList[0:i]))))
			self.fileHandle.write("\n\n")

		member = members.getMember(namespace)
		if member:
			self.createMember(namespace, member, "## ")

	def createListing(self, namespace, memberList):
		prevKind = -1
		for member in memberList:
			kind = member.getKind()
			if kind != prevKind:
				self.fileHandle.write("### {}\n".format(member.getDefinition()["name"]))
				self.fileHandle.write("||||\n")
				self.fileHandle.write("|---:|:---|:---|\n")

			self.fileHandle.write("|{}|[`{}`]({})|{}|\n".format(
					member.printDefinition("{type}"),
					member.printDefinition("{name}"),
					self.generateLink(namespace, member),
					member.getDescriptionBrief()))
			prevKind = kind

	def formatComment(self, comment):

		pattern = re.compile("<([^>]{1,6})>")

		index = 0
		output = ""
		currentStack = ["comment"]

		def formatText(current, text):
			if current == "comment":
				return text.replace("\n", "\n\n")
			return text

		for match in pattern.finditer(comment):

			# Format the text add it to the output
			output += formatText(currentStack[-1], comment[index:match.start()])

			index = match.end()

			# Identify the command and set the current state
			command = match.group(1).strip()
			if command == "code":
				currentStack.append("code")
				output += "```"
			elif command == "/code":
				currentStack.pop()
				output += "```"
			# False positive, print all
			else:
				output += match.group(0)

		# Format the trailing text
		output += formatText(currentStack[-1], comment[index:])

		return output

	def createMember(self, namespace, member, preTitle):
		self.fileHandle.write("{}`{} {}`\n".format(
				preTitle,
				member.printDefinition("{template} {pre} {type}"),
				member.printDefinition("{name} {post}")))

		if member.getProvenance():
			self.fileHandle.write("*From {}*\n\n".format(member.getProvenance()))

		self.fileHandle.write("{}\n".format(self.formatComment(member.getDescription())))

		definition = member.getDefinition()

		# Set the template
		if len(member.getTemplate()):
			self.fileHandle.write("#### Template\n")
			self.fileHandle.write("||||\n")
			self.fileHandle.write("|---:|:---|:---|\n")
			for template in member.getTemplate():
				self.fileHandle.write("|{}|{}|{}|\n".format(template.get("type"), template.get("name"), template.get("description", "")))

		# Set the arguments
		if definition["args"] and len(member.getArgs()):
			self.fileHandle.write("#### Parameters\n")
			self.fileHandle.write("||||\n")
			self.fileHandle.write("|---:|:---|:---|\n")
			for arg in member.getArgs():
				self.fileHandle.write("|{}|{}|{}|\n".format(arg.get("type"), arg.get("name"), arg.get("description", "")))

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

	def process(self, members):
		for namespace, memberGroup in members.items():
			memberList = memberGroup.get()

			self.useFile(namespace)

			self.createNamespace(namespace, members)

			self.createListing(namespace, memberList)
			for member in memberList:
				self.fileHandle.write("------\n")
				self.createMember(namespace, member, "### ")
				#print(" - %s" % (member.printDefinition()))
				#print(" - %s" % (member.getInheritance()))

			self.closeFile()
