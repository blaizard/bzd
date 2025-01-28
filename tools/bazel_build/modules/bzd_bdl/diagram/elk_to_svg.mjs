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
			"elk.algorithm": "layered",
			"elk.direction": "RIGHT",
			"elk.nodeSize.constraints": "NODE_LABELS PORTS PORT_LABELS MINIMUM_SIZE",
			"elk.nodeLabels.placement": "V_TOP H_CENTER INSIDE",
			"elk.nodeLabels.padding": "top=0, right=50, bottom=0, left=20",
			"elk.portConstraints": "FIXED_ORDER",
			"elk.portLabels.placement": "INSIDE",
			"elk.edgeLabels.placement": "CENTER",
			"elk.layered.edgeLabels.sideSelection": "DIRECTION_DOWN",
			"elk.spacing.edgeLabel": 20,
			"elk.spacing.labelPortHorizontal": 20,
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
		const group = svg.group().id(edge.id);
		for (const section of edge.sections || []) {
			let points = [section.startPoint, section.endPoint];
			if (section.bendPoints) {
				points.splice(1, 0, ...section.bendPoints);
			}
			group.polyline(points.map((point) => [point.x + x, point.y + y]));
		}
		for (const name of ["edge", ...edge.classes]) {
			group.addClass(name);
		}
	}

	drawNode(svg, node, x, y) {
		const group = svg.group().id(node.id);
		group.rect(node.width, node.height);
		for (const name of ["node", ...node.classes]) {
			group.addClass(name);
		}
		group.move(x, y);

		this.drawChildren(group, node, x, y);
	}

	drawLabel(svg, label, x, y) {
		if (label.svg) {
			label.svg.move(x, y);
			svg.add(label.svg);
		}
	}

	drawPort(svg, port, x, y) {
		const group = svg.group().id(port.id);
		group.rect(port.width, port.height).addClass("port");
		group.move(x, y);

		this.drawChildren(group, port, x, y);
	}

	drawChildren(svg, graph, offsetX, offsetY) {
		for (const node of graph.children || []) {
			this.drawNode(svg, node, node.x + offsetX, node.y + offsetY);
		}

		for (const edge of graph.edges || []) {
			this.drawEdge(svg, edge, offsetX, offsetY);
		}

		for (const port of graph.ports || []) {
			this.drawPort(svg, port, port.x + offsetX, port.y + offsetY);
		}

		for (const label of graph.labels || []) {
			this.drawLabel(svg, label, label.x + offsetX, label.y + offsetY);
		}
	}

	/// Some elements needs to be preprocessed, like the labels for example,
	/// as they do not have a size.
	preprocess(svg, json) {
		for (const label of json.labels || []) {
			if (label.text) {
				const svgText = svg.text((add) => {
					for (const line of label.text.split("\n")) {
						add.tspan(line);
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
		svg.viewbox(0, 0, graph.width, graph.height);

		// Set the style.
		svg.style(".edge", {
			stroke: "rgb(0, 148, 133)",
			fill: "none",
		});
		svg.style(".edge.io", {
			"stroke-width": "2",
		});
		svg.style(".edge.dependency", {
			"stroke-width": "1",
			"stroke-dasharray": "4",
		});
		svg.style("text", {
			fill: "#000",
			stroke: "none",
		});
		svg.style(".node", {
			fill: "#eee",
			stroke: "rgb(0, 148, 133)",
			"stroke-width": "1",
			color: "#000",
		});
		svg.style(".node.level-0", {
			fill: "#fafafa",
		});
		svg.style(".node.level-1", {
			fill: "#eaeaea",
		});
		svg.style(".node.level-2", {
			fill: "#e0e0e0",
		});
		svg.style(".port", {
			fill: "rgb(0, 148, 133)",
			stroke: "rgb(0, 148, 133)",
			"stroke-width": "1",
		});

		this.drawChildren(svg, graph, 0, 0);
		return svg.svg();
	}
}
