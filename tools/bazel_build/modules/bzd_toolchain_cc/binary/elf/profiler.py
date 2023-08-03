#!/usr/bin/python

import argparse
import json
import re
import os
import sys
from typing import Any, Tuple, Dict, List
from elftools.elf.elffile import ELFFile


def processElfDwarf(path: str) -> Tuple[Dict[str, int], List[str]]:
	compilers = set()
	compilerList: List[str] = []

	with open(path, "rb") as f:
		elffile = ELFFile(f)
		result: Dict[str, int] = {}

		if not elffile.has_dwarf_info():
			return result, compilerList

		dwarfInfo = elffile.get_dwarf_info()
		for CU in dwarfInfo.iter_CUs():
			topDIE = CU.get_top_DIE()
			name = topDIE.get_full_path()

			# Update the CU size
			size = dieInfoRec(topDIE)
			if size:
				if name not in result:
					result[name] = 0
				result[name] += size

			# Gather compilers
			if topDIE.tag == "DW_TAG_compile_unit":
				if "DW_AT_producer" in topDIE.attributes:
					compiler = (topDIE.attributes["DW_AT_producer"].value.decode("ascii", errors="ignore").lower())
					# Remove everything after the first argument
					index = compiler.find(" -")
					if index != -1:
						compiler = compiler[0:index]
					compilers.add(compiler)

	compilerList = list(compilers)
	compilerList.sort()

	return result, compilerList


def dieInfoRec(die: Any) -> int:
	size = 0

	# Compute the size of the current DIE
	if "DW_AT_high_pc" in die.attributes:
		highPC = die.attributes["DW_AT_high_pc"]
		if highPC.form == "DW_FORM_addr":
			assert "DW_AT_low_pc" in die.attributes, "Missing low_pc"
			size = highPC.value - die.attributes["DW_AT_low_pc"].value
		else:
			size = highPC.value

	# Recursively get children's DIE size, only if parent does not have size information
	if size == 0:
		for child in die.iter_children():
			size += dieInfoRec(child)

	return size


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Binary profiler.")
	parser.add_argument("--format", choices=["elf-dwarf"], default="elf-dwarf", help="Binary format.")
	parser.add_argument("--groups", default=None, help="Groups schema if any.")
	parser.add_argument("binary", help="Path of the binary to profile.")
	parser.add_argument("binary_final", help="Path of the final binary to profile.")
	parser.add_argument("output", help="Path of the json output.")

	args = parser.parse_args()

	groups = {}
	if args.groups:
		with open(args.groups, "r") as f:
			groupsData = json.load(f)
			# Compute regular expression from the groups
			groups = {re.compile(match): group for match, group in groupsData.items()}

	if args.format == "elf-dwarf":
		result, compilers = processElfDwarf(args.binary)
	else:
		raise Exception("Unsupported format.")

	groupedResult = {}

	for path, size in result.items():
		matchedGroup = None
		for regexpr, group in groups.items():
			m = regexpr.search(path)
			if m:
				assert matchedGroup == None, "Path matched at least 2 groups."
				matchedGroup = group.format(**m.groupdict())
		matchedGroup = matchedGroup if matchedGroup else path
		if matchedGroup not in groupedResult:
			groupedResult[matchedGroup] = 0
		groupedResult[matchedGroup] += size

	finalResult = {
	    "size_groups": {
	        "units": groupedResult,
	    },
	    "size": os.path.getsize(args.binary_final),
	    "compilers": compilers,
	}

	with open(args.output, "w+") as f:
		f.write(json.dumps(finalResult, indent=4, sort_keys=True))

	sys.exit(0)
