#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import re

from lib.utility import assertCommand, executeCommand

class Configuration():

	def __init__(self, workspace, config, tools = {}):
		self.workspace = workspace
		self.config = config
		self.execRoot = executeCommand(["./tools/bazel", "info", "execution_root"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.bazelBin = executeCommand(["./tools/bazel", "info", "bazel-bin"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.ignoreList = [
			re.compile(r"^external/"),
			re.compile(r"^bazel-out/"),
			re.compile(r".*\.S$")
		]
		# Set the tools
		self.tools = {}
		for name, tool in tools.items():
			assert isinstance(tool, dict), "Tools must contains only dictionary entries."
			assert "cmd" in tool, "Tool is missing 'cmd' key"
			assert "cmdAssert" in tool, "Tool is missing 'cmdAssert' key"
			assertCommand(self.sanitizeCmd(tool["cmdAssert"]))
			self.tools[name] = self.sanitizeCmd(tool["cmd"])

	def sanitizeCmd(self, cmd):
		return [path.replace("%EXEC_ROOT%", self.execRoot) for path in cmd]

	def getToolCmd(self, name):
		assert name in self.tools, "This tool is not registered"
		return self.tools[name]

	def getBazelExtraArgs(self):
		return ["--config", self.config] if self.config else []

	def getExecRoot(self):
		return self.execRoot

	def getBazelBin(self):
		return self.bazelBin

	def getWorkspace(self):
		return self.workspace

	def isValidPath(self, path):
		for ignoreRegexpr in self.ignoreList:
			if ignoreRegexpr.match(path):
				return False
		return True
