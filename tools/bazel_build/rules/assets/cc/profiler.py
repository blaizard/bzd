#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import argparse
import json
import re
import sys
from elftools.elf.elffile import ELFFile

def processFile(path):

    with open(path, "rb") as f:
        elffile = ELFFile(f)
        result = {}

        if not elffile.has_dwarf_info():
            return result

        dwarfInfo = elffile.get_dwarf_info()
        for CU in dwarfInfo.iter_CUs():

            topDIE = CU.get_top_DIE()
            name = topDIE.get_full_path()

            # Update the CU size
            size = dieInfoRec(topDIE)
            if size:
                if name not in result:
                    result[name] = {
                        "size": 0
                    }
                result[name]["size"] += size

    return result


def dieInfoRec(die):
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

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description = "Binary profiler.")
    parser.add_argument("--format", choices = ["elf-dwarf"], default = "elf-dwarf", help = "Binary format.")
    parser.add_argument("--groups", default = None, help = "Groups schema if any.")
    parser.add_argument("binary", help = "Path of the binary to profile.")

    args = parser.parse_args()

    if args.format == "elf-dwarf":
        result = processFile(args.binary)
    else:
        raise Exception("Unsupported format.")

    groups = {
        "/libm/": "libm",
        "/libc/": "libc",
        "/libgcc/": "libgcc",
        "/libstdc\\+\\+": "libstdc++",
        "/hal/hal/": "hal",
        "/esp-idf/components/(?P<component>[^/]+)/": "{component}"
    }

    groupedResult = {}
    size = 0

    # Compute regular expression from the groups
    groups = {re.compile(match): group for match, group in groups.items()}

    for path, data in result.items():
        size += data["size"]
        matchedGroup = None
        for regexpr, group in groups.items():
            m = regexpr.search(path)
            if m:
                assert matchedGroup == None, "Path matched at least 2 groups."
                matchedGroup = group.format(**m.groupdict())
        if matchedGroup:
            if matchedGroup not in groupedResult:
                groupedResult[matchedGroup] = {
                    "size": 0
                }
            groupedResult[matchedGroup]["size"] += data["size"]
        else:
            groupedResult[path] = data

    print(json.dumps(groupedResult))
    print(size)