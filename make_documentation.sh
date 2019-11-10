#!/bin/bash

set -e

doxygen
./tools/documentation/parsedocumentation.py
