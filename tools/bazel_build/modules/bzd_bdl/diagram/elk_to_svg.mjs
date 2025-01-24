import ELK from "elkjs";
import { SVG, registerWindow } from "@svgdotjs/svg.js";
import { createSVGWindow } from "svgdom";

/// Translate an elk definition into SVG.
export default class ElkToSVG {
	constructor() {
		// The following is needed with nodejs to emulate a DOM.
		const window = createSVGWindow();
		const document = window.document;
		registerWindow(window, document);

		this.svg = SVG(document.documentElement);
		this.layoutOptions = {
			"elk.direction": "RIGHT",
			"elk.nodeSize.constraints": "NODE_LABELS PORTS PORT_LABELS MINIMUM_SIZE",
			"elk.nodeLabels.placement": "V_TOP H_CENTER INSIDE",
			"elk.portConstraints": "FIXED_ORDER",
			"elk.portLabels.placement": "INSIDE",
		};
	}

	async render(json) {
		this.preprocess(this.svg, json);

		const elk = new ELK();
		return await elk
			.layout(json, {
				layoutOptions: this.layoutOptions,
			})
			.then((graph) => this.draw(graph))
			.catch((e) => {
				console.error(e);
				throw new Error("ELK layout failed.");
			});
	}

	drawEdge(svg, edge, x, y) {
		const group = svg.group().id(edge.id).move(x, y);
		for (const section of edge.sections) {
			let points = [section.startPoint, section.endPoint];
			if (section.bendPoints) {
				points.splice(1, 0, ...section.bendPoints);
			}
			group.polyline(points.map((point) => [point.x, point.y])).addClass("edge");
		}
	}

	drawNode(svg, node, x, y) {
		const group = svg.group().id(node.id);
		group.rect(node.width, node.height).addClass("node");
		group.move(x, y);

		this.drawChildren(group, node, x, y);
	}

	drawLabel(svg, label, x, y) {
		if (label.text) {
			const group = svg.group();
			group.text(label.text);
			group.move(x, y);
		}
	}

	drawPort(svg, port, x, y) {
		const group = svg.group().id(port.id);
		group.rect(port.width, port.height).addClass("port");
		group.move(x, y);

		this.drawChildren(svg, port, x, y);
	}

	drawChildren(svg, graph, x, y) {
		for (const node of graph.children || []) {
			this.drawNode(svg, node, node.x + x, node.y + y);
		}

		for (const edge of graph.edges || []) {
			this.drawEdge(svg, edge, edge.x + x, edge.y + y);
		}

		for (const port of graph.ports || []) {
			this.drawPort(svg, port, port.x + x, port.y + y);
		}

		for (const label of graph.labels || []) {
			this.drawLabel(svg, label, label.x + x, label.y + y);
		}
	}

	/// Some elements needs to be preprocessed, like the labels for example,
	/// as they do not have a size.
	preprocess(svg, json) {
		for (const label of json.labels || []) {
			if (label.text) {
				const svgText = svg.text((add) => {
					for (const line of label.text.split("\n")) {
						const tspan = add.tspan(line);
						tspan.dy(tspan.bbox().height);
					}
				});
				const bbox = svgText.bbox();
				Object.assign(label, {
					svg: svgText,
					width: bbox.width,
					height: bbox.height,
				});
			}
		}

		for (const attribute of ["edges", "ports", "children"]) {
			for (const nested of json[attribute] || []) {
				this.preprocess(svg, nested);
			}
		}
	}

	draw(graph) {
		const svg = this.svg.width(graph.width).height(graph.height);

		// Set the style.
		svg.style(".edge", {
			stroke: "#f06",
			"stroke-width": "2",
			fill: "none",
		});
		svg.style(".node", {
			fill: "#eee",
			stroke: "#f06",
			"stroke-width": "1",
		});
		svg.style(".port", {
			fill: "#f06",
			stroke: "#f06",
			"stroke-width": "1",
		});

		this.drawChildren(svg, graph, graph.x, graph.y);
		return svg.svg();
	}
}
