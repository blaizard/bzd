export default class Filter {
  constructor(filters) { this.filters = filters.map((filterStr) => Filter._compile(filterStr)); }

  static _escapeRegExp(string) { return string.replace(/[.*+\-?^${}()|[\]\\]/g, "\\$&"); }

  static _compile(filterStr) {
	let index = 0;
	let regexpr = "^";
	while (index < filterStr.length) {
	  const c = filterStr[index];
	  if (c == "*") {
		if (index + 1 < filterStr.length && filterStr[index + 1] == "*") {
		  regexpr += ".*";
		  ++index;
		} else {
		  regexpr += "[^/]*";
		}
	  } else {
		regexpr += Filter._escapeRegExp(c);
	  }
	  ++index;
	}
	return new RegExp(regexpr + "$");
  }

  match(path) {
	for (const regexpr of this.filters) {
	  if (path.match(regexpr)) {
		return true;
	  }
	}
	return false;
  }
}
