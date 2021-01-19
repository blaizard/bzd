#!/usr/bin/python

import argparse
import json
import re
import os
import sys
from typing import Any, Tuple, Dict, List

from fixed_table import FixedTable

"""
Sections in a map file:
	- Archive member included because of file (symbol)
	- Discarded input sections: List of sections and symbols that are discarded.
	- Memory map

	- Linker script and memory map

START GROUP
LOAD external/esp32_xtensa_lx6_sdk_4.0/lib/libapp_trace.a
LOAD external/esp32_xtensa_lx6_sdk_4.0/lib/libapp_update.a
LOAD external/esp32_xtensa_lx6_sdk_4.0/lib/libasio.a
LOAD external/esp32_xtensa_lx6_sdk_4.0/lib/libbootloader_sup


or

Memory map

 ** dynamic relocs
                0x000000000000c798      0x198

.init           0x000000000000c930       0x17
 .init          0x000000000000c930       0x12 /usr/lib/gcc/x86_64-linux-gnu/7/../../../x86_64-linux-gnu/crti.o
                0x000000000000c930                _init
 .init          0x000000000000c942        0x5 /usr/lib/gcc/x86_64-linux-gnu/7/../../../x86_64-linux-gnu/crtn.o

.plt            0x000000000000c950      0x120
 ** PLT         0x000000000000c950      0x120

.text           0x000000000000ca70     0x6142
 .text          0x000000000000ca70       0x2b /usr/lib/gcc/x86_64-linux-gnu/7/../../../x86_64-linux-gnu/Scrt1.o
                0x000000000000ca70                _start

<section> <address> <size> <file>(<module>)
 .text._ZN3bzd6AtomicIPNS_4impl11ListElementILb1EEEEC1ERKS4_
                0x0000000000000000       0x43 bazel-out/k8-fastbuild/bin/cc/bzd/core/libscheduler.a(scheduler.pic.o)
"""

"""
clang map file format:

             VMA              LMA     Size Align Out     In      Symbol
          2026a0           2026a0      3d6    16 .rodata
          2026a0           2026a0        4     4         <internal>:(.rodata)
          2026a4           2026a4      287     1         <internal>:(.rodata)
          20292b           20292b        1     1         bazel-out/k8-fastbuild_linux_x86_64_clang/bin/example/liblib.a(main.pic.o):(.rodata._ZN3bzd4implL6await_E)
          20292b           20292b        1     1                 bzd::impl::await_
"""

class _Data:
	def __init__(self, units, sections):
		self.units = self._aggregateUnits(units)
		self.sections = self._aggregateSections(sections)

	@staticmethod
	def _aggregateSections(sections):
		aggregatedSections = {}
		# Combine sections
		for item in sections:
			if item["section"] not in aggregatedSections:
				aggregatedSections[item["section"]] = 0
			aggregatedSections[item["section"]] += item["size"]
		# Remove empty sections
		return {section: size for section, size in aggregatedSections.items() if size > 0}

	@staticmethod
	def _aggregateUnits(units):
		aggregatedUnits = {}
		for entry in units:
			if entry["units"] not in aggregatedUnits:
				aggregatedUnits[entry["units"]] = {}
			data = aggregatedUnits[entry["units"]]
			if entry["section"] not in data:
				data[entry["section"]] = 0
			data[entry["section"]] += entry["size"]
		# Remove empty sections
		for unit, sections in aggregatedUnits.items():
			aggregatedUnits[unit] = {section: size for section, size in sections.items() if size > 0}
		return aggregatedUnits

	def getByUnits(self):
		return self.units

	def getBySections(self):
		return self.sections

	@staticmethod
	def _patternToRegexpr(pattern):
		regexpr = r''
		for fragment in pattern.split("*"):
			regexpr += re.escape(fragment) if fragment else r'.*'
		return re.compile(regexpr)

	def filter(self, pattern):
		regexpr = self._patternToRegexpr(pattern)
		# Filter sections
		self.sections = {section: data for section, data in self.sections.items() if not regexpr.fullmatch(section)}
		# Filter units
		for unit, sections in self.units.items():
			self.units[unit] = {section: data for section, data in sections.items() if not regexpr.fullmatch(section)}

def parseMapClang(f):

	headersSchema = {
		"vma": { "align": "right" },
		"lma": { "align": "right" },
		"size": { "align": "right" },
		"align": { "align": "right" },
		"out": { "align": "left" },
		"in": { "align": "left" },
		"symbol": { "align": "left" },
	}

	# First line corresponds to the formating.
	table = FixedTable.fromHeader(header = f.readline().lower(), schema = headersSchema)
	print(table.nbColumns())

	if table.nbColumns() < 4:
		return None

	section = None
	units = []
	sections = []
	# Parse the content of the table
	for line in f:
		parts = table.parse(line)

		# Extract important data
		cellOut = parts.get("out")
		if cellOut:
			section = cellOut
			sections.append({
				"section": section,
				"size": int(parts.get("size"), 16)
			})

		cellIn = parts.get("in")
		if cellIn:
			m = re.match(r'^([^\(:]+).*$', cellIn)
			if m:
				units.append({
					"section": section,
					"size": int(parts.get("size"), 16),
					"units": m.group(1)
				})

	return _Data(units = units, sections = sections)

def goto(f, regexpr):
	"""
	Go to a line that matches the regular expression,
	if not found, return False.
	"""
	for line in f:
		if regexpr.match(line):
			return True
	return False

def parseMapGcc(f):

	# Look for a line that ends with "memory map"
	if not goto(f, re.compile(r'^.*memory map$', re.IGNORECASE)):
		return False

	for line in f:
		table = FixedTable.fromPattern(line, re.compile(r'^(.*)\s(0x[0-9a-f]{8,})\s(\s+0x[0-9a-f]+)\s(.*)$', re.IGNORECASE))
		if table:
			break

	# Go back to the begining
	f.seek(0)
	goto(f, re.compile(r'^.*memory map$', re.IGNORECASE))

	sections = []
	units = []
	section = None
	isSection = False
	for line in f:
		parts = table.parse(line)
		if parts.empty():
			continue

		# Identify the current section
		maybeSection = parts.get(0)
		if maybeSection:
			# Sub sections are one space off, ignore them
			if parts.get(0, strip = False)[0] != " ":
				section = maybeSection
				isSection = True

		# If the data only contains the section, ignore as we have already handled it
		if parts.size() == 1:
			continue

		# Size must at least be defined to go further
		maybeSize = parts.get(2)
		if not maybeSize:
			continue

		# Populate the section
		if section and isSection:
			sections.append({
				"section": section,
				"size": int(maybeSize, 16)
			})
		isSection = False

		# Populate the unit
		maybeUnit = parts.get(3)
		if maybeUnit:
			if parts.get(3, strip = False)[1] != " ":
				m = re.match(r'^([^\(:]+).*$', maybeUnit)
				units.append({
					"section": section,
					"size": int(maybeSize, 16),
					"units": m.group(1) if m else "undefined"
				})

		#print("|{}|{}|{}|{}|".format(parts.get(0), parts.get(1), parts.get(2), parts.get(3)))
	print(units)
	print(sections)

	return _Data(units = units, sections = sections)

"""
 .text._ZN3bzd3minImJEEET_RKS1_S3_DpRKT0_
                0x000000000000d8a6       0x37 bazel-out/k8-fastbuild/bin/example/liblib.a(main.pic.o)
                0x000000000000d8a6                unsigned long bzd::min<unsigned long>(unsigned long const&, unsigned long const&)
 ** fill        0x000000000000d8dd        0x1
"""
"""
 .data.rel.local.DW.ref.__gxx_personality_v0
                0x0000000000019010        0x8 bazel-out/k8-fastbuild/bin/example/liblib.a(main.pic.o)
                0x0000000000019010                DW.ref.__gxx_personality_v0
"""

def formatData(data, filter = [], aggregate = {}):

	# Filter sections, most likely sections that will be stripped

	return data

# section address size file module

if __name__ == '__main__':

	parser = argparse.ArgumentParser(description="Map file analyzer.")
	parser.add_argument("map", help="Path of the map file to analyze.")
	parser.add_argument("output", help="Path of the json output.")

	args = parser.parse_args()

	for parser in [parseMapGcc, parseMapClang]:
		with open(args.map, "r") as f:
			data = parser(f)
			if data:
				break

	# Filter sections, most likely sections that will be stripped
	data.filter(".debug_*")
	data.filter(".comment")
	data.filter(".symtab")

	final = {
		"units": data.getByUnits(),
		"sections": data.getBySections()
	}

	print(json.dumps(final, sort_keys=True, indent=4))

	with open(args.output, "w+") as f:
		f.write(json.dumps(final, sort_keys=True, indent=4))

	sys.exit(0)
