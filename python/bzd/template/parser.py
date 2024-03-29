import typing

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import (
    Fragment,
    FragmentNestedStart,
    FragmentNestedStopNewElement,
    FragmentNewElement,
    FragmentParentElement,
)
from bzd.parser.element import Element

# Match all remaining content
_regexprContent = r"(?P<content>((?!{[{%#]).)+?)(?={[{%#])"
# Match escape characters.
_regexprEscape = r"{(?P<value>{[{%#])"
# Strip left of control and comments blocks until the first newline
_regexprContentStripAuto = r"(?P<content>((?!{[{%#]).)*?\n)[ \t]*(?={[%#])"
# Strip right of content when followed by a left stripped block
_regexprContentStripRight = r"(?P<content>((?!{[{%#]).)*?)\s*(?={[{%#]-)"
# Match all remaining content until the end of the file
_regexprContentEndOfFile = r"(?P<content>[\s\S]+?)(?=$)"
# Match an identifier
_regexprIdentifier = r"(?P<name>([a-zA-Z_\-0-9]+))"
# Match condition
_regexprCondition = r"(?P<condition>(.+?(?=-?%})))"
# Match comment
_regexprComment = r"(?P<comment>(.+?(?=-?#})))"
# Number
_regexprNumber = r"(?P<value>-?[0-9]+(?:\.[0-9]*)?)"
# String
_regexprString = r"\"(?P<value>.*?(?<!\\))\""
# Boolean true
_regexprBooleanTrue = r"(?P<value>true)"
# Boolean false
_regexprBooleanFalse = r"(?P<value>false)"


def makeRegexprSymbol(name: str) -> str:
	"""
    Match a name.
    """
	return r"(?P<" + name + r">([a-zA-Z_\-0-9]+))"


def makeGrammarExpression(name: str, continuation: Grammar, attrs: typing.Dict[str, str] = {}) -> Grammar:
	"""Grammar for an expression type, which can be either an integer,
    a boolean, a string or a symbol.
    It supports arrays, pipes, callable and nested expressions.

    For example, the following would be valid and would generate the following element:

    hello.you.yes("ds", 12, yes["4"].sd | no).d.sds | hi | no[12]

    - <Element context="<parent>"/>
    hello:
    - <Element type:1:1="symbol" context="<parent>"/>
            symbol:
            - <Element value:1:6="hello" context="<parent>"/>
            - <Element value:7:10="you" context="<parent>"/>
            - <Element value:11:14="yes" context="<parent>"/>
            - <Element type:14:15="callable" context="<parent>"/>
            callable:
            - <Element type:15:19="string" value:15:19="ds" context="<parent>"/>
            - <Element context="<parent>"/>
            - <Element type:21:23="number" value:21:23="12" context="<parent>"/>
            - <Element context="<parent>"/>
            - <Element type:25:25="symbol" context="<parent>"/>
                    symbol:
                    - <Element value:25:28="yes" context="<parent>"/>
                    - <Element type:28:29="array" context="<parent>"/>
                    array:
                    - <Element type:29:32="string" value:29:32="4" context="<parent>"/>
                    - <Element context="<parent>"/>
                    - <Element value:34:36="sd" context="<parent>"/>
                    - <Element context="<parent>"/>
            - <Element type:37:38="pipe" context="<parent>"/>
            - <Element type:39:39="symbol" context="<parent>"/>
                    symbol:
                    - <Element value:39:41="no" context="<parent>"/>
                    - <Element context="<parent>"/>
            - <Element context="<parent>"/>
            - <Element value:43:44="d" context="<parent>"/>
            - <Element value:45:48="sds" context="<parent>"/>
            - <Element context="<parent>"/>
    - <Element type:49:50="pipe" context="<parent>"/>
    - <Element type:51:51="symbol" context="<parent>"/>
            symbol:
            - <Element value:51:53="hi" context="<parent>"/>
            - <Element context="<parent>"/>
    - <Element type:54:55="pipe" context="<parent>"/>
    - <Element type:56:56="symbol" context="<parent>"/>
            symbol:
            - <Element value:56:58="no" context="<parent>"/>
            - <Element type:58:59="array" context="<parent>"/>
            array:
            - <Element type:59:61="number" value:59:61="12" context="<parent>"/>
            - <Element context="<parent>"/>
            - <Element context="<parent>"/>

    """

	class ExpressionStart(FragmentNestedStart):
		nestedName = name
		default = attrs

	return [
	    GrammarItem(
	        None,
	        ExpressionStart,
	        makeGrammarExpressionImpl([GrammarItem(None, FragmentParentElement, continuation)]),
	    )
	]


def makeGrammarExpressionImpl(continuation: Grammar) -> Grammar:

	class SymbolStart(FragmentNestedStart):
		nestedName = "symbol"
		default = {"type": "symbol"}

	class CallableStart(FragmentNestedStart):
		nestedName = "callable"
		default = {"type": "callable"}

	class ArrayStart(FragmentNestedStart):
		nestedName = "array"
		default = {"type": "array"}

	def makeFragmentNewElement(elementType: str) -> typing.Type[FragmentNewElement]:

		class Custom(FragmentNewElement):
			default = {"type": elementType}

		return Custom

	class FragmentOperator(FragmentNewElement):

		def process(self, match: typing.Match[str]) -> None:
			self.attrs["type"] = match.group(0)

	symbol: Grammar = []
	symbolTransition: Grammar = []
	symbolNext: Grammar = []
	openParenthesis: Grammar = []
	closeParenthesisOrNext: Grammar = []
	openArray: Grammar = []
	closeArray: Grammar = []

	openParenthesis.extend([
	    GrammarItemSpaces,
	    GrammarItem(r"\)", FragmentNestedStopNewElement, symbolTransition),
	    lambda: makeGrammarExpressionImpl(closeParenthesisOrNext),
	])

	closeParenthesisOrNext.extend([
	    GrammarItemSpaces,
	    GrammarItem(r",", FragmentNewElement, openParenthesis),
	    GrammarItem(r"\)", FragmentNestedStopNewElement, symbolTransition),
	])

	openArray.extend([lambda: makeGrammarExpressionImpl(closeArray)])

	closeArray.extend([
	    GrammarItemSpaces,
	    GrammarItem(r"\]", FragmentNestedStopNewElement, symbolTransition),
	])

	finalGrammar = [
	    GrammarItemSpaces,
	    GrammarItem(
	        r"(\||==|!=|<=|>=|<|>|in)",
	        FragmentOperator,
	        [lambda: makeGrammarExpressionImpl(continuation)],
	    ),
	] + continuation

	symbol.extend([
	    GrammarItemSpaces,
	    GrammarItem(makeRegexprSymbol("value"), FragmentNewElement, symbolTransition),
	])
	symbolTransition.extend([
	    GrammarItemSpaces,
	    GrammarItem(r"\(", CallableStart, openParenthesis),
	    GrammarItem(r"\[", ArrayStart, openArray),
	    GrammarItem(r"\.", {}, symbolNext),
	    GrammarItem(None, FragmentNestedStopNewElement, finalGrammar),
	])
	symbolNext.extend(symbol)

	return [
	    GrammarItemSpaces,
	    GrammarItem(r"not", makeFragmentNewElement("not")),
	    GrammarItem(_regexprNumber, makeFragmentNewElement("number"), finalGrammar),
	    GrammarItem(_regexprString, makeFragmentNewElement("string"), finalGrammar),
	    GrammarItem(_regexprBooleanTrue, makeFragmentNewElement("true"), finalGrammar),
	    GrammarItem(_regexprBooleanFalse, makeFragmentNewElement("false"), finalGrammar),
	    GrammarItem(None, SymbolStart, symbol),
	]


def makeGrammarContent() -> Grammar:
	"""
    Generate a grammar for the raw content.
    """

	class FragmentContent(FragmentNewElement):
		default = {"category": "content"}

	return [
	    # Note the order is important here.
	    GrammarItem(_regexprContentStripRight, FragmentContent),
	    GrammarItem(_regexprContentStripAuto, FragmentContent),
	    GrammarItem(_regexprContent, FragmentContent),
	    GrammarItem(_regexprContentEndOfFile, FragmentContent),
	]


def makeGrammarSubstitutionStart(grammar: Grammar) -> Grammar:
	return [GrammarItem(r"{{-?", Fragment, grammar)]


def makeGrammarControlStart(keyword: str, grammar: Grammar) -> Grammar:
	return [
	    GrammarItem(r"(^[ \t]*)?{%-?\s*" + keyword, Fragment, grammar),
	]


def makeGrammarCommentStart(grammar: Grammar) -> Grammar:
	return [
	    GrammarItem(r"(^[ \t]*)?{#-?", Fragment, grammar),
	]


def makeGrammarSubstitutionStop(fragment: typing.Type[Fragment]) -> Grammar:
	return [
	    GrammarItem(r"(?=}})", Fragment, [GrammarItem(r"}}", fragment)]),
	    GrammarItem(r"(?=-}})", Fragment, [GrammarItem(r"-}}\s*", fragment)]),
	]


def makeGrammarControlStop(
    fragment: typing.Type[Fragment],
    grammar: typing.Optional[typing.Union[Grammar, str]] = None,
) -> Grammar:
	return [
	    GrammarItem(
	        r"(?=%})",
	        Fragment,
	        [GrammarItem(r"%}([ \t]*\n[ \t]*(?={[%#])|[ \t]*\n|[. \t]*$)?", fragment, grammar)],
	    ),
	    GrammarItem(r"(?=-%})", Fragment, [GrammarItem(r"-%}\s*", fragment, grammar)]),
	]


def makeGrammarCommentStop(fragment: typing.Type[Fragment]) -> Grammar:
	return [
	    GrammarItem(
	        r"(?=#})",
	        Fragment,
	        [GrammarItem(r"#}([ \t]*\n[ \t]*(?={[%#])|[ \t]*\n|[. \t]*$)?", fragment)],
	    ),
	    GrammarItem(r"(?=-#})", Fragment, [GrammarItem(r"-#}\s*", fragment)]),
	]


def makeGrammarSubstitution() -> Grammar:
	"""
    Generate a grammar for substitution blocks.
    """

	return makeGrammarSubstitutionStart([
	    GrammarItem(
	        None,
	        {"category": "substitution"},
	        [makeGrammarExpression("value", [makeGrammarSubstitutionStop(FragmentNewElement)])],
	    )
	])


def makeGrammarControlInclude() -> Grammar:
	"""
    Generate the grammar for the include block.
    It matches the following:
            include value %}
    """
	return makeGrammarControlStart(
	    r"include",
	    [GrammarItem(
	        _regexprString,
	        {"category": "include"},
	        makeGrammarControlStop(FragmentNewElement),
	    )],
	)


def makeGrammarControlFor() -> Grammar:
	"""
    Generate the grammar for the for loop control block.
    It matches the following:
            for value1 [, value2] in iterable %}
    """

	grammarFromIn = [
	    GrammarItem(
	        r"in",
	        Fragment,
	        makeGrammarExpression("iterable", makeGrammarControlStop(FragmentNestedStart, "root")),
	    )
	]

	return makeGrammarControlStart(
	    r"for",
	    [
	        GrammarItem(
	            None,
	            {"category": "for"},
	            [
	                GrammarItem(
	                    makeRegexprSymbol("value1"),
	                    Fragment,
	                    [
	                        GrammarItem(
	                            r",",
	                            Fragment,
	                            [GrammarItem(
	                                makeRegexprSymbol("value2"),
	                                Fragment,
	                                grammarFromIn,
	                            )],
	                        )
	                    ] + grammarFromIn,
	                )
	            ],
	        )
	    ],
	)


def makeGrammarControlIf() -> Grammar:
	"""
    Generate the grammar for the if block.
    It matches the following:
            if condition %}
    """
	return makeGrammarControlStart(
	    r"if",
	    [
	        GrammarItem(
	            None,
	            {"category": "if"},
	            makeGrammarExpression("condition", makeGrammarControlStop(FragmentNestedStart, "root")),
	        )
	    ],
	)


def makeGrammarControlElseIf() -> Grammar:
	"""
    Generate the grammar for the else / elif block.
    It matches the following:
            else %}
            elif condition %}
    """

	class ElseFragment(FragmentNestedStart):
		default = {"category": "else"}

	return [
	    GrammarItem(
	        r"(?={%-?\s*(else|elif))",
	        FragmentNestedStopNewElement,
	        makeGrammarControlStart(
	            r"elif",
	            [
	                makeGrammarExpression(
	                    "condition",
	                    makeGrammarControlStop(FragmentNestedStart, "root"),
	                    {"category": "else"},
	                )
	            ],
	        ) + makeGrammarControlStart(r"else", makeGrammarControlStop(ElseFragment, "root")),
	    )
	]


def makeGrammarControlMacro() -> Grammar:
	"""
    Generate the grammar for a macro block.
    It matches the following:
            macro name(arg1, arg2, ...) %}
    """

	class ArgumentStart(FragmentNestedStart):
		nestedName = "argument"
		default = {"category": "macro"}

	return makeGrammarControlStart(
	    r"macro",
	    [
	        GrammarItem(
	            _regexprIdentifier + r"\(",
	            ArgumentStart,
	            [
	                GrammarItem(
	                    makeRegexprSymbol("name"),
	                    Fragment,
	                    [
	                        GrammarItem(r",", FragmentNewElement),
	                        GrammarItem(
	                            r"\)",
	                            FragmentParentElement,
	                            makeGrammarControlStop(FragmentNestedStart, "root"),
	                        ),
	                    ],
	                ),
	                GrammarItem(
	                    r"\)",
	                    FragmentParentElement,
	                    makeGrammarControlStop(FragmentNestedStart, "root"),
	                ),
	            ],
	        )
	    ],
	)


def makeGrammarControlEnd() -> Grammar:
	"""
    Generate the grammar for the end control block.
    """

	class FragmentEndElement(FragmentNewElement):
		default = {"category": "end"}

	return [
	    GrammarItem(
	        r"(?={%-?\s*end)",
	        FragmentParentElement,
	        makeGrammarControlStart(
	            r"end",
	            [GrammarItem(
	                None,
	                FragmentNewElement,
	                makeGrammarControlStop(FragmentEndElement),
	            )],
	        ),
	    )
	]


def makeGrammarControl() -> Grammar:
	"""
    Generate the grammar for all control blocks.
    """

	return [
	    GrammarItem(
	        r"(?={%)",
	        Fragment,
	        makeGrammarControlFor() + makeGrammarControlIf() + makeGrammarControlElseIf() + makeGrammarControlMacro() +
	        makeGrammarControlInclude() + makeGrammarControlEnd(),
	    )
	]


def makeGrammarComments() -> Grammar:
	"""
    Generate the grammar code comments.
    """

	return makeGrammarCommentStart(
	    [GrammarItem(
	        _regexprComment,
	        {"category": "comment"},
	        makeGrammarCommentStop(FragmentNewElement),
	    )])


def makeGrammarEscape() -> Grammar:
	"""
    Generate the grammar for escaping code blocks.
    """

	class FragmentEscape(FragmentNewElement):
		default = {"category": "escape"}

	return [GrammarItem(_regexprEscape, FragmentEscape)]


class Parser(ParserBase):

	def __init__(self, content: str) -> None:
		super().__init__(
		    content,
		    grammar=makeGrammarEscape() + makeGrammarSubstitution() + makeGrammarControl() + makeGrammarComments() +
		    makeGrammarContent(),
		    defaultGrammarPost=[GrammarItemSpaces],
		)
