import argparse
import json
import sys
import pathlib
import typing

from elftools.elf.elffile import ELFFile

from binary.map_analyzer.parser.clang import ParserClang
from binary.map_analyzer.parser.gcc import ParserGcc
from binary.map_analyzer.parser.parser import Parser

FilterConfigType = typing.List[str]
AggregateConfigType = typing.Dict[str, typing.List[str]]


def generateBerkeleyConfig(
    filePath: pathlib.Path, ) -> typing.Optional[typing.Tuple[FilterConfigType, AggregateConfigType]]:
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
	aggregateConfig: AggregateConfigType = {
	    # text/code segment.
	    "text": [],
	    # data segment: typically contains initallized variables.
	    "data": [],
	    # block starting symbol: typically contains uninitallized variables.
	    "bss": [],
	}

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


def analyze(data: Parser) -> None:
	"""
    Analyze the data gathered from the linker map.
    """

	# Build a list sorted by addresses
	byAddresses: typing.Dict[int, typing.Dict[str, typing.Any]] = {}
	for section, obj in data.getBySections().items():
		address = obj.get("address")
		if address:
			if address in byAddresses:
				print(
				    "Sections '{}' and '{}' are located at the same address: {:#08x}.".format(
				        section, byAddresses[address]["section"], address),
				    file=sys.stderr,
				)
				continue
			byAddresses[address] = {
			    "size": obj.get("size"),
			    "address": address,
			    "section": section,
			}
	sortedByAddresses: typing.List[typing.Dict[str, typing.Any]] = sorted([obj for address, obj in byAddresses.items()],
	                                                                      key=lambda x: x["address"])  # type: ignore

	# Ensure there is no overlaps
	addressEnd = 0
	previousSection = None
	for item in sortedByAddresses:
		address = item["address"]
		size = item["size"]
		section = item["section"]

		assert isinstance(address, int)
		if address < addressEnd:
			print(
			    "Section '{}'@{:#08x} overlaps with previous section '{}'.".format(section, address, previousSection),
			    file=sys.stderr,
			)

		addressEnd = address + size
		previousSection = section
		# print("{:#08x} {:#08x} {:8} {:32}".format(address, addressEnd, size, section))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Map file analyzer.")
	parser.add_argument("-o", "--output", default="map.json", help="Path of the json output.")
	parser.add_argument(
	    "-b",
	    "--binary",
	    type=pathlib.Path,
	    help="Build configuration from the binary file provided.",
	)
	parser.add_argument("map", type=pathlib.Path, help="Path of the map file to analyze.")

	args = parser.parse_args()

	for ParserType in [ParserGcc, ParserClang]:
		data = ParserType(args.map)
		if data.parse():
			break

	# Remove some of the sections, which are known to be special.
	# Thread local sections that overlaps other section such as .init_array, see:
	# https://stackoverflow.com/questions/25501044/gcc-ld-overlapping-sections-tbss-init-array-in-statically-linked-elf-bin
	data.filter(".tbss")
	data.filter(".tdata")

	# Analyze the data
	analyze(data)

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
		            "sections": data.getByAggregatedSections(),
		        }},
		        indent=4,
		        sort_keys=True,
		    ))

	sys.exit(0)
