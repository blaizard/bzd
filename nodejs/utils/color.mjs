import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("color");

export default class Color {
	constructor(str) {
		this.data = this._parse(str);
	}

	_normalizeColor(str) {
		let ctx = document.createElement("canvas").getContext("2d");
		ctx.fillStyle = str;
		return ctx.fillStyle;
	}

	_parse(str) {
		const normalizedStr = this._normalizeColor(str);

		if (normalizedStr.length == 4) {
			return {
				r: parseInt("0x" + normalizedStr[1] + normalizedStr[1]),
				g: parseInt("0x" + normalizedStr[2] + normalizedStr[2]),
				b: parseInt("0x" + normalizedStr[3] + normalizedStr[3]),
				a: 1
			};
		}
		else if (normalizedStr.length == 7) {
			return {
				r: parseInt("0x" + normalizedStr[1] + normalizedStr[2]),
				g: parseInt("0x" + normalizedStr[3] + normalizedStr[4]),
				b: parseInt("0x" + normalizedStr[5] + normalizedStr[6]),
				a: 1
			};
		}
		Exception.unreachable("Unsupported color format: '{}'", normalizedStr);
	}

	setAlpha(alpha) {
		this.data.a = alpha;
	}

	toString() {
		return "rgba(" + this.data.r + ", " + this.data.g + ", " + this.data.b + ", " + this.data.a + ")";
	}
}
