import { SVG } from "@svgdotjs/svg.js";
import ElkToSVG from "./library.js";

/// Translate an elk definition into SVG.
export default function elkToSVG() {
	const svg = SVG();
	return new ElkToSVG(svg);
}
