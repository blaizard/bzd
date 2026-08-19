import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("color");

export default {
	normalize: (color: string): string => {
        const context = document.createElement("canvas").getContext("2d");
        Exception.assert(context, "Unable to create a 2D canvas context");
        const ctx = context as CanvasRenderingContext2D;
        ctx.fillStyle = color;
        return ctx.fillStyle;
    }
}
