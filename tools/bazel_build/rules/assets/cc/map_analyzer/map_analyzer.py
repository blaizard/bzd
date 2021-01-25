import argparse
import json
import sys
import pathlib
import typing

from elftools.elf.elffile import ELFFile

from tools.bazel_build.rules.assets.cc.map_analyzer.parser.clang import ParserClang
from tools.bazel_build.rules.assets.cc.map_analyzer.parser.gcc import ParserGcc

FilterConfigType = typing.List[str]
AggregateConfigType = typing.Dict[str, typing.List[str]]


def generateBerkeleyConfig(
		filePath: pathlib.Path) -> typing.Optional[typing.Tuple[FilterConfigType, AggregateConfigType]]:
	"""
	Classify sections to match berkeley respresentation:
		remove: !SEC_ALLOC
		text: SEC_CODE || SEC_READONLY
		data: SEC_HAS_CONTENTS
		bss: other

	Those flags can be obtained with objdump:
	objdump -h bazel-bin/example/example.stripped

	See implementation from GNU size:
	static void berkeley_sum (bfd *abfd ATTRIBUTE_UNUSED, sec_ptr sec,
			void *ignore ATTRIBUTE_UNUSED)
	"""

	filterConfig: FilterConfigType = []
	aggregateConfig: AggregateConfigType = {"text": [], "data": [], "bss": []}

	FLAG_SHF_WRITE = 0x1
	FLAG_SHF_ALLOC = 0x2
	FLAG_SHF_EXECINSTR = 0x4

	with open(filePath, "rb") as f:
		elffile = ELFFile(f)
		for section in elffile.iter_sections():

			if not section.name:
				continue

			# Generate useful flags to categorize the sections
			isReadonly = not bool(section["sh_flags"] & FLAG_SHF_WRITE)
			isAlloc = bool(section["sh_flags"] & FLAG_SHF_ALLOC)
			isCode = bool(section["sh_flags"] & FLAG_SHF_EXECINSTR)
			hasContent = section["sh_type"] not in ["SHT_NOBITS"]

			sectionType = None
			if not isAlloc:
				filterConfig.append(section.name)
			else:
				if isCode or isReadonly:
					aggregateConfig["text"].append(section.name)
				elif hasContent:
					aggregateConfig["data"].append(section.name)
				else:
					aggregateConfig["bss"].append(section.name)

		return filterConfig, aggregateConfig


if __name__ == '__main__':

	parser = argparse.ArgumentParser(description="Map file analyzer.")
	parser.add_argument("-o", "--output", default="map.json", help="Path of the json output.")
	parser.add_argument("-b", "--binary", type=pathlib.Path, help="Build configuration from the binary file provided.")
	parser.add_argument("map", type=pathlib.Path, help="Path of the map file to analyze.")

	args = parser.parse_args()

	for ParserType in [ParserGcc, ParserClang]:
		data = ParserType(args.map)
		if data.parse():
			break

	# If a binary is provided, open it and generate configuration from it.
	filterConfig: FilterConfigType = [".debug_*", ".comment", ".symtab", ".strtab"]
	aggregateConfig: AggregateConfigType = {}
	if args.binary:
		result = generateBerkeleyConfig(args.binary)
		if result:
			filterConfig, aggregateConfig = result

	# Filter sections, most likely sections that will be stripped
	for sectionName in filterConfig:
		data.filter(sectionName)

	# Add aggregated patterns
	for name, patternList in aggregateConfig.items():
		data.addAggregation(name, patternList)

	with open(args.output, "w+") as f:
		f.write(
			json.dumps(
			{"size_groups": {
			"units": data.getByAggregatedUnits(),
			"sections": data.getByAggregatedSections()
			}}))

	sys.exit(0)
