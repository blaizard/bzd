import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Backend from "#bzd/nodejs/utils/impl/color/backend.js";

const Exception = ExceptionFactory("color");

interface ColorData {
    r: number;
    g: number;
    b: number;
    a: number;
}

export default class Color {
    private data: ColorData;

    constructor(str: string) {
        this.data = this._parse(str);
    }

    /// Parse a color string into its rgba components.
    _parse(str: string): ColorData {
        const normalizedStr = Backend.normalize(str);

        if (normalizedStr.length == 4) {
            return {
                r: parseInt("0x" + normalizedStr[1] + normalizedStr[1]),
                g: parseInt("0x" + normalizedStr[2] + normalizedStr[2]),
                b: parseInt("0x" + normalizedStr[3] + normalizedStr[3]),
                a: 1,
            };
        } else if (normalizedStr.length == 7) {
            return {
                r: parseInt("0x" + normalizedStr[1] + normalizedStr[2]),
                g: parseInt("0x" + normalizedStr[3] + normalizedStr[4]),
                b: parseInt("0x" + normalizedStr[5] + normalizedStr[6]),
                a: 1,
            };
        }
        return Exception.unreachable("Unsupported color format: '{}'", normalizedStr);
    }

    /// Set the alpha channel of the color.
    setAlpha(alpha: number): void {
        this.data.a = alpha;
    }

    toString(): string {
        return "rgba(" + this.data.r + ", " + this.data.g + ", " + this.data.b + ", " + this.data.a + ")";
    }
}