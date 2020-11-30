import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("color");

export default class Color {
	constructor(str) {
		this.data = this._parse(str);
	}

	_parse(str) {
		if (str.length == 4) {
			return {
				r: parseInt("0x" + str[1] + str[1]),
				g: parseInt("0x" + str[2] + str[2]),
				b: parseInt("0x" + str[3] + str[3]),
				a: 1,
			};
		}
		else if (str.length == 7) {
			return {
				r: parseInt("0x" + str[1] + str[2]),
				g: parseInt("0x" + str[3] + str[4]),
				b: parseInt("0x" + str[5] + str[6]),
				a: 1,
			};
		}
		Exception.unreachable("Unsupported color format: '{}'", str);
	}

	setAlpha(alpha) {
		this.data.a = alpha;
	}

	toString() {
		return "rgba(" + this.data.r + ", " + this.data.g + ", " + this.data.b + ", " + this.data.a + ")";
	}
}
