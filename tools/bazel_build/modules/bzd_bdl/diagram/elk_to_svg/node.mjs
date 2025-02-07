import { SVG, registerWindow } from "@svgdotjs/svg.js";
import { createSVGWindow } from "svgdom";
import ElkToSVG from "./library.mjs";

/// Translate an elk definition into SVG.
export default function elkToSVG() {
	// The following is needed with nodejs to emulate a DOM.
	const window = createSVGWindow();
	const document = window.document;
	registerWindow(window, document);
	const svg = SVG(document.documentElement);
	return new ElkToSVG(svg);
}
