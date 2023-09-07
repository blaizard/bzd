/// From https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions
export function regexprEscape(string) {
	return string.replace(/[.*+\-?^${}()|[\]\\]/g, "\\$&");
}
