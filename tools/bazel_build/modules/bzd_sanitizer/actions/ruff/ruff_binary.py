"""Thin wrapper to invoke the native ruff binary from its pip wheel."""

import pathlib
import os
import sys

import ruff

ruff_binary = pathlib.Path(ruff.__file__).parent.parent.parent / "bin" / "ruff"
if not ruff_binary.is_file():
    raise FileNotFoundError(f"The binary 'ruff' could not be found at {ruff_binary}")
os.execv(ruff_binary, [str(ruff_binary), *sys.argv[1:]])
