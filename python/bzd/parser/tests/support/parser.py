from pathlib import Path

from bzd.parser.parser import Parser
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import (
    Fragment,
    FragmentNestedStart,
    FragmentNestedStopNewElement,
    FragmentNewElement,
    FragmentComment,
)

# Simple Grammar

_grammarComment = [GrammarItem(r"//(?P<comment>[^\n]*)", FragmentComment)]
_grammarVariableDeclaration = [
    GrammarItem(r"const", {"const": ""}),
    GrammarItem(
        r"(?P<kind>int|float)",
        {"variable": ""},
        [
            GrammarItem(
                r"(?P<name>[a-zA-Z_]+)",
                Fragment,
                [
                    GrammarItem(r";", FragmentNewElement),
                    GrammarItem(
                        r"=",
                        Fragment,
                        [GrammarItem(
                            r"(?P<value>[0-9]+)",
                            Fragment,
                            [
                                GrammarItem(r";", FragmentNewElement),
                            ],
                        )],
                    ),
                ],
            ),
        ],
    ),
]
_grammarFunctionDeclaration = [
    GrammarItem(
        r"(?P<return>void|int|float)",
        {"function": ""},
        [
            GrammarItem(
                r"(?P<name>[a-zA-Z_]+)\s*\(\s*\)",
                Fragment,
                [
                    GrammarItem(
                        r"{",
                        FragmentNestedStart,
                        _grammarVariableDeclaration + [GrammarItem(r"}", FragmentNestedStopNewElement)],
                    )
                ],
            )
        ],
    ),
]

_grammar: Grammar = _grammarVariableDeclaration + _grammarFunctionDeclaration


class TestParser(Parser):  # type: ignore

	def __init__(self, content: str) -> None:
		super().__init__(
		    content,
		    grammar=_grammar,
		    defaultGrammarPre=[GrammarItemSpaces] + _grammarComment,
		)
