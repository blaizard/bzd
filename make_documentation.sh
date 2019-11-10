#!/bin/bash

set -e

rm -rfd ./docs/xml
doxygen
./tools/documentation/parsedocumentation.py
