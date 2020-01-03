#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import re

from lib.utility import assertCommand, executeCommand

class Configuration():

	def __init__(self, workspace, config, tools = {}):
		self.workspace = workspace
		self.config = config
		self.execRoot = executeCommand(["bazel", "info", "execution_root"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.bazelBin = executeCommand(["bazel", "info", "bazel-bin"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.outputBase = executeCommand(["bazel", "info", "output_base"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.tools = {name: tool.replace("%EXEC_ROOT%", self.execRoot) for name, tool in tools.items()}
		self.ignoreList = [
			re.compile(r"^external/"),
			re.compile(r"^bazel-out/"),
			re.compile(r".*\.S$")
		]

	def getTool(self, name):
		assert name in self.tools, "This tool is not registered"
		return self.tools[name]

	def getBazelExtraArgs(self):
		return ["--config", self.config] if self.config else []

	def getExecRoot(self):
		return self.execRoot

	def getBazelBin(self):
		return self.bazelBin

	def getOutputBase(self):
		return self.outputBase

	def getWorkspace(self):
		return self.workspace

	def isValidPath(self, path):
		for ignoreRegexpr in self.ignoreList:
			if ignoreRegexpr.match(path):
				return False
		return True
