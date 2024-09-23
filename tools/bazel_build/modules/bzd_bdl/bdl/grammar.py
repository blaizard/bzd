import re
import typing
from pathlib import Path

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import (
    Fragment,
    FragmentNestedStart,
    FragmentNestedStopNewElement,
    FragmentNewElement,
    FragmentParentElement,
    FragmentComment,
)
from bzd.parser.element import Element

_regexprBaseName = r"(?!const\b|interface\b|struct\b|component\b|method\b|namespace\b|use\b|using\b|config\b|composition\b|[0-9])[0-9a-zA-Z_]+"
# Match name: abc
_regexprName = r"(?P<name>" + _regexprBaseName + r")"
# Match name or varargs: abc or abc...
_regexprNameOrVarArgs = r"(?P<name>" + _regexprBaseName + r"(?:\.\.\.)?)"
# Match: "string", 12, -45, 5.1854, true, false
_regexprValue = r"(?P<value>\".*?(?<!\\)\"|-?[0-9]+(?:\.[0-9]*)?|true|false)"
# Match string: "hello"
_regexprString = r"\"(?P<value>.*?)\""
# Match regular expression: /.*/
_regexprRegexpr = r"/(?P<regexpr>[^\s]+)/"
# Match preset: {abc.def}
_regexprPreset = (r"\{(?P<preset>" + _regexprBaseName + r"(?:\." + _regexprBaseName + r")*)\}")
# Match operator: + or - or \ or *
_regexprOperator = r"(?P<operator>[\+\-\\\*])"


# Match any type of symbol except protected types
def _makeRegexprFQN(name: str) -> str:
	return (r"(?P<" + name + r">" + _regexprBaseName + r"(?:\." + _regexprBaseName + r")*)")


class FragmentBlockComment(FragmentComment):

	def process(self, match: typing.Match[str]) -> None:
		assert "comment" in self.attrs, "Missing comment attribute."
		self.attrs["comment"] = re.sub(re.compile(r"^(\s*\*)+", re.MULTILINE), "", self.attrs["comment"])


def makeGrammarNested(
    nestedGrammar: Grammar,
    name: str,
    defaultNested: str = "invalid",
    nested: typing.Optional[typing.Set[str]] = None,
) -> Grammar:
	"""
    Generate a grammar for a nested entity, it accepst the following format:
    (interface|struct|component|composition) [name] [contracts] [: inheritance1, inheritance2, ...] {
            nestedGrammar
    }

    Nested type elements are included under `nested`.
    """

	class InheritanceStart(FragmentNestedStart):
		nestedName = "inheritance"

	class NestedInterface(FragmentNestedStart):
		nestedName = defaultNested

	def makeNestedCategory(name: str) -> GrammarItem:

		class CategoryFragmentStart(FragmentNestedStart):
			nestedName = name

		return GrammarItem(
		    name + r"(?=:)",
		    FragmentParentElement,
		    [GrammarItem(r":", CategoryFragmentStart, "nested")],
		)

	if nested is None:
		nested = set()

	grammarAfterName: Grammar = [
	    GrammarItem(
	        r":",
	        InheritanceStart,
	        [
	            GrammarItem(
	                _makeRegexprFQN("interface"),
	                Fragment,
	                [
	                    GrammarItem(r",", FragmentNewElement),
	                    GrammarItem(r"(?={)", FragmentParentElement),
	                ],
	            )
	        ],
	    ),
	    GrammarItem(
	        r"{",
	        NestedInterface,
	        nestedGrammar + [makeNestedCategory(nestedName) for nestedName in nested] + [
	            GrammarItem(r"}", FragmentNestedStopNewElement),
	        ],
	        "nested",
	    ),
	] + makeGrammarContracts()

	return [
	    GrammarItem(
	        r"(?P<category>" + name + r")",
	        {},
	        [GrammarItem(_regexprName, Fragment, grammarAfterName)] + grammarAfterName,
	    )
	]


def makeGrammarSymbol(nextGrammar: Grammar) -> Grammar:
	"""
    Generate a grammar for Symbol, it accepts the following format:
    Type = [const] Type1[<Type, Type, ...>]

    Nested type elements are included under `template`.
    """

	class TemplateStart(FragmentNestedStart):
		nestedName = "template"

	def makeGrammar(continuation: Grammar, nested: typing.Optional[Grammar] = None) -> Grammar:
		grammar: Grammar = [GrammarItem(r"const", {"const": ""})]
		grammar.append(
		    GrammarItem(
		        _makeRegexprFQN("symbol"),
		        Fragment,
		        [
		            GrammarItem(r"<", TemplateStart, [grammar if nested is None else nested]),
		            continuation,
		        ],
		    ))
		return grammar

	nested = makeGrammar([
	    GrammarItem(r",", FragmentNewElement),
	    GrammarItem(r">", FragmentParentElement),
	])

	topLevel = makeGrammar(nextGrammar, nested)

	return topLevel


def makeGrammarContracts(name: str = "contract") -> Grammar:
	"""
    Generate a grammar for Contracts, it accepts the following format:
    [Type1[(value1, value2, ...)], Type2[(value1, value2, ...)] ...]

    Nested type elements are included under `contracts`.
    """

	class ContractStart(FragmentNestedStart):
		nestedName = name

	class ValuesStart(FragmentNestedStart):
		nestedName = "values"

	return [
	    GrammarItem(
	        r"\[",
	        ContractStart,
	        [
	            GrammarItem(
	                _makeRegexprFQN("type"),
	                Fragment,
	                [
	                    GrammarItem(
	                        r"\(",
	                        ValuesStart,
	                        [
	                            GrammarItem(
	                                r"(?P<value>[\-\.0-9a-zA-Z_]+)",
	                                Fragment,
	                                [
	                                    GrammarItem(r",", FragmentNewElement),
	                                    GrammarItem(r"\)", FragmentParentElement),
	                                ],
	                            ),
	                        ],
	                    ),
	                    GrammarItem(r"\]", FragmentParentElement),
	                    GrammarItem(r"", FragmentNewElement),
	                ],
	            )
	        ],
	    ),
	]


def makeGrammarExpressionFragment(finalGrammar: Grammar = [GrammarItem(r";", FragmentNewElement)]) -> Grammar:
	"""
    Generate a grammar for an expression, it accepts the following format:
    (symbol|value)[(arg1, arg2, ...)] (+|-) (symbol|value)[(arg1, arg2, ...)] ... [contract];

    where argX is a recursive call to this grammar.
    """

	class FragmentsStart(FragmentNestedStart):
		nestedName = "fragments"

	class ArgumentStart(FragmentNestedStart):
		nestedName = "argument"

	def makeGrammar(continuation: Grammar, nested: typing.Optional[Grammar] = None) -> Grammar:
		grammarValue: Grammar = []
		grammarWrapper: Grammar = [GrammarItem("", FragmentsStart, grammarValue)]

		# Test for regular expression (must be before operator as it will match with '/').
		grammarValue.append(GrammarItem(_regexprRegexpr, FragmentNewElement, grammarValue))

		# Test for operator.
		grammarValue.append(GrammarItem(_regexprOperator, FragmentNewElement, grammarValue))

		# Test for value.
		grammarValue.append(GrammarItem(_regexprValue, FragmentNewElement, grammarValue))

		# Test for preset.
		grammarValue.append(GrammarItem(_regexprPreset, FragmentNewElement, grammarValue))

		# Test for symbol.
		grammarValue.extend(
		    makeGrammarSymbol([
		        GrammarItem(
		            r"\(",
		            ArgumentStart,
		            [
		                # Empty parenthesis will not have nested fragments, which will simplify the parsing.
		                GrammarItem(r"\)", FragmentParentElement),
		                GrammarItem(
		                    "",
		                    {"category": "expression"},
		                    [
		                        GrammarItem(_regexprName + r"\s*="),
		                        grammarWrapper if nested is None else nested,
		                    ],
		                ),
		            ],
		        ),
		        GrammarItem("", FragmentNewElement, grammarValue),
		    ]))

		# End
		grammarValue.append(GrammarItem("", FragmentParentElement, continuation))

		return grammarWrapper

	nestedGrammar = makeGrammar([GrammarItem(",", FragmentNewElement), GrammarItem(r"\)", FragmentParentElement)])
	return makeGrammar([makeGrammarContracts(), finalGrammar], nestedGrammar)


def makeGrammarVariable(finalGrammar: Grammar = [GrammarItem(r";", FragmentNewElement)]) -> Grammar:
	"""
    Generate a grammar for Variables, it accepts the following format:
    name [: interface] = symbol[(value)] [contract];
    """

	return [
	    GrammarItem(
	        _regexprNameOrVarArgs + r"\s*(:\s*" + _makeRegexprFQN("interface") + r"\s*)?=",
	        {"category": "expression"},
	        makeGrammarExpressionFragment(finalGrammar),
	    )
	]


def makeGrammarExpression() -> Grammar:
	"""
    Generate a grammar for Variables, it accepts the following format:
    [name [: interface] =] symbol[(value)] [contract];
    """

	finalGrammar: Grammar = [GrammarItem(r";", FragmentNewElement)]

	return makeGrammarVariable(finalGrammar) + [
	    GrammarItem(
	        r"(?=" + _regexprBaseName + r")",
	        {"category": "expression"},
	        makeGrammarExpressionFragment(finalGrammar),
	    )
	]


def makeGrammarMethod() -> Grammar:
	"""
    Generate a grammar for methods, it accepts the following format:
    method name([inputs...]) [contract] [-> returntype [contract]];
    """

	class ArgumentStart(FragmentNestedStart):
		nestedName = "argument"

	return [
	    GrammarItem(
	        r"method",
	        {"category": "method"},
	        [
	            GrammarItem(
	                _regexprName,
	                Fragment,
	                [
	                    GrammarItem(
	                        r"\(",
	                        ArgumentStart,
	                        [
	                            makeGrammarVariable([
	                                GrammarItem(r",", FragmentNewElement),
	                                GrammarItem(r"\)", FragmentParentElement),
	                            ]),
	                            GrammarItem(r"\)", FragmentParentElement),
	                        ],
	                    ),
	                    makeGrammarContracts(),
	                    GrammarItem(
	                        r"->",
	                        Fragment,
	                        [
	                            makeGrammarSymbol([
	                                makeGrammarContracts(name="contract_return"),
	                                GrammarItem(r";", FragmentNewElement),
	                            ])
	                        ],
	                    ),
	                    GrammarItem(r";", FragmentNewElement),
	                ],
	            )
	        ],
	    )
	]


def makeGrammarUsing() -> Grammar:
	"""
    Generate a grammar for using keyword, it accepts the following format:
    using name = Type [contract];
    """

	return [
	    GrammarItem(
	        r"using",
	        {"category": "using"},
	        [
	            GrammarItem(
	                _regexprName,
	                Fragment,
	                [
	                    GrammarItem(
	                        r"=",
	                        Fragment,
	                        makeGrammarSymbol([
	                            makeGrammarContracts(),
	                            GrammarItem(r";", FragmentNewElement),
	                        ]),
	                    )
	                ],
	            )
	        ],
	    )
	]


def makeGrammarExtern() -> Grammar:
	"""
    Generate a grammar for extern keyword, it accepts the following format:
    extern category symbol;
    """

	return [
	    GrammarItem(
	        r"extern",
	        {"extern": "true"},
	        [
	            GrammarItem(
	                r"(?P<category>(:?interface|struct))",
	                Fragment,
	                [GrammarItem(
	                    _regexprName,
	                    Fragment,
	                    [GrammarItem(r";", FragmentNewElement)],
	                )],
	            )
	        ],
	    )
	]


def makeGrammarEnum() -> Grammar:
	"""
    Generate a grammar for an enum, it accepts the following format:
    enum name { VALUE1 [,VALUE2 [,...]] }
    """

	class EnumStart(FragmentNestedStart):
		nestedName = "values"

	return [
	    GrammarItem(
	        r"enum",
	        {"category": "enum"},
	        [
	            GrammarItem(
	                _regexprName,
	                Fragment,
	                [
	                    GrammarItem(
	                        r"{",
	                        EnumStart,
	                        [
	                            GrammarItem(
	                                _regexprName,
	                                Fragment,
	                                [
	                                    GrammarItem(r",", FragmentNewElement),
	                                    GrammarItem(r"}", FragmentNestedStopNewElement),
	                                ],
	                            )
	                        ],
	                    )
	                ],
	            )
	        ],
	    )
	]


def makeGrammarNamespace() -> Grammar:
	"""
    Generate a grammar for namespace, it accepts the following format:
    namespace level1[.level2[.level3...]];
    """

	class NamespaceStart(FragmentNestedStart):
		default = {"category": "namespace"}
		nestedName = "name"

	return [
	    GrammarItem(
	        r"namespace",
	        NamespaceStart,
	        [
	            GrammarItem(
	                _regexprName,
	                Fragment,
	                [
	                    GrammarItem(r"\.", FragmentNewElement),
	                    GrammarItem(r";", FragmentNestedStopNewElement),
	                ],
	            )
	        ],
	    )
	]


def makeGrammarUse() -> Grammar:
	"""
    Generate a grammar for use, it accepts the following format:
    use "path/to/file"
    """
	return [GrammarItem(
	    r"use",
	    {"category": "use"},
	    [GrammarItem(_regexprString, FragmentNewElement)],
	)]


# Comments allowed by the grammar
_grammarComments = [
    GrammarItem(r"/\*(?P<comment>([\s\S]*?))\*/", FragmentBlockComment),
    GrammarItem(r"//(?P<comment>[^\n]*)", FragmentComment),
]


class Parser(ParserBase):

	def __init__(self, content: str) -> None:
		withinNested = (makeGrammarUsing() + makeGrammarEnum() + makeGrammarExpression() + makeGrammarMethod())
		nested = withinNested

		super().__init__(
		    content,
		    grammar=makeGrammarNamespace() + makeGrammarUse() + makeGrammarExtern() + withinNested + makeGrammarNested(
		        withinNested,
		        name="component",
		        nested={"interface", "config", "composition"},
		    ) + makeGrammarNested(withinNested, name="interface", defaultNested="interface") +
		    makeGrammarNested(withinNested, name="composition", defaultNested="composition") +
		    makeGrammarNested(withinNested, name="struct", defaultNested="interface"),
		    defaultGrammarPre=[GrammarItemSpaces] + _grammarComments,
		)
