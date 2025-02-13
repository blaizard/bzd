import elkToSVG from "./elk_to_svg/entry_point.mjs";
import BdlToElk from "./bdl_to_elk.mjs";

export default class BdlToSvg {
	constructor(bdls) {
		let converter = new BdlToElk();
		this.targets = new Set();
		for (const bdl of bdls) {
			converter.addTarget(bdl);
			this.targets.add(bdl.target);
		}
		this.elk = converter.process();
	}

	identifyNodesToShow(node, options, level = 0) {
		if (level == 2) {
			for (const target of options.ignore || []) {
				if (node.id.startsWith(target)) {
					return [];
				}
			}
		}

		let targets = [node.id];
		for (const child of node.children || []) {
			targets = targets.concat(this.identifyNodesToShow(child, options, level + 1));
		}
		return targets;
	}

	filter(node, options, ids = new Set(), level = 0) {
		// Set the known target identifiers.
		ids.add(node.id);
		for (const port of node.ports || []) {
			ids.add(port.id);
		}

		// Filter out children.
		const children = (node.children || [])
			.filter((child) => {
				return options.targets.has(child.id);
			})
			.map((child) => this.filter(child, options, ids, level + 1));

		// Update the edges to keep only valid ones.
		const edges = (node.edges || []).filter((edge) => {
			for (const target of [...edge.sources, ...edge.targets]) {
				if (!ids.has(target)) {
					return false;
				}
			}
			return true;
		});

		return Object.assign({}, node, {
			edges: edges,
			children: children,
		});
	}

	async render(all) {
		const targets = this.identifyNodesToShow(this.elk, { ignore: all ? [] : this.targets });
		const elkFiltered = this.filter(this.elk, {
			targets: new Set(targets),
		});

		return await elkToSVG().render(elkFiltered);
	}
}
