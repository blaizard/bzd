import bzd.env
import argparse
import sys
import os
import logging
import pathlib
import typing
from functools import partial
import importlib.util

from bzd.utils.file import Watcher
import bzd.parser.error

defaultContent = """from pathlib import Path

from bzd.parser.parser import Parser
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStopNewElement, FragmentNewElement, FragmentComment

# ==== Grammar ================================================================

_grammarVariableDeclaration = [
	GrammarItem(r"const", {"const": ""}),
	GrammarItem(r"(?P<kind>int|float)", {"variable": ""}, [
	GrammarItem(r"(?P<name>[a-zA-Z_]+)", Fragment, [
	GrammarItem(r";", FragmentNewElement),
	GrammarItem(r"=", Fragment,
	[GrammarItem(r"(?P<value>[0-9]+)", Fragment, [
	GrammarItem(r";", FragmentNewElement),
	])])
	]),
	]),
]
_grammarFunctionDeclaration = [
	GrammarItem(r"(?P<return>void|int|float)", {"function": ""}, [
	GrammarItem(r"(?P<name>[a-zA-Z_]+)\s*\(\s*\)", Fragment, [
	GrammarItem(r"{", FragmentNestedStart,
	_grammarVariableDeclaration + [GrammarItem(r"}", FragmentNestedStopNewElement)])
	])
	]),
]

_grammar: Grammar = _grammarVariableDeclaration + _grammarFunctionDeclaration

# ==== Parser =================================================================

class TestParser(Parser):

	def __init__(self, content: str) -> None:
		super().__init__(content, grammar=_grammar, defaultGrammarPre=[GrammarItemSpaces], defaultGrammarPost=[])

# ==== Test Data ==============================================================

output = TestParser(\"""
void hello() {}
\""").parse()
"""


def clearTerminal() -> None:
	os.system("cls || clear")


def changed(lib: pathlib.Path) -> None:
	spec = importlib.util.spec_from_file_location("scratch_pad", lib)
	assert spec is not None
	scratchPad = importlib.util.module_from_spec(spec)
	assert spec.loader is not None
	clearTerminal()
	try:
		spec.loader.exec_module(scratchPad)
		print(f"=== Output of {lib}")
		print(scratchPad.output)
	except Exception as e:
		print("ERROR")
		print(e)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Playground to write grammar schema for parsing.")
	parser.add_argument("path", type=pathlib.Path, help="File to watch.")
	args = parser.parse_args()

	bzd.parser.error.useColors = True

	assert "BUILD_WORKSPACE_DIRECTORY" in os.environ, "This binary must run with bazel."
	path = (pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"]).resolve(strict=True) / args.path)

	if not path.is_file():
		path.write_text(defaultContent)
		logging.info(f"Creating empty file: {path}")

	logging.info(f"Watching file: {path}")
	Watcher(path, changed).watch()

	sys.exit(0)
