#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

from .clang_format import clangFormat
from .eslint import eslint

actionsList = {
    "clang-format": clangFormat,
    "eslint": eslint
}
