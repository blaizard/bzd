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
		[this.nodeIds, this.edges] = this.getNodeIds(this.elk);
		this.addEdgesToNodeIds();
	}

	/// Find the closest node from a given identifier.
	findClosestNodeFromId(id) {
		let ids = id.split(".");
		while (ids) {
			const potentialId = ids.join(".");
			if (potentialId in this.nodeIds) {
				return potentialId;
			}
			ids.pop();
		}
		throw new Exception("Cannot find the closest target for id: " + id);
	}

	addEdgesToNodeIds() {
		for (const edge of this.edges) {
			const source = this.findClosestNodeFromId(edge.begin);
			const end = this.findClosestNodeFromId(edge.end);

			if (edge.tags.includes("io")) {
				this.nodeIds[source].ios.add(end);
				this.nodeIds[end].ios.add(source);
			}
			if (edge.tags.includes("dependency")) {
				this.nodeIds[source].deps.add(end);
				this.nodeIds[end].deps.add(source);
			}
		}
	}

	/// Read all node identifiers, including nested nodes and their corresponding nested level.
	getNodeIds(node, level = 0) {
		// Identify the direct connections.
		let edges = [];
		for (const edge of node.edges || []) {
			for (const id1 of [...edge.sources]) {
				for (const id2 of [...edge.targets]) {
					edges.push({
						begin: id1,
						end: id2,
						tags: edge.classes || [],
					});
				}
			}
		}

		let targets = {
			[node.id]: {
				level: level,
				ios: new Set(),
				deps: new Set(),
			},
		};
		for (const child of node.children || []) {
			const [childTargets, childEdges] = this.getNodeIds(child, level + 1);
			Object.assign(targets, childTargets);
			edges = edges.concat(childEdges);
		}
		return [targets, edges];
	}

	identifyNodesToShow(options) {
		return Object.entries(this.nodeIds)
			.filter(([id, data]) => {
				if (data.level == 2) {
					for (const idIgnore of options.ignore || []) {
						if (id.startsWith(idIgnore)) {
							return false;
						}
					}
				}
				return true;
			})
			.map(([id, data]) => {
				// Also include direct dependencies
				return [id, ...data.ios];
			})
			.flat();
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
		const targets = this.identifyNodesToShow({ ignore: all ? [] : this.targets });
		const elkFiltered = this.filter(this.elk, {
			targets: new Set(targets),
		});
		const deepCopyElkFiltered = structuredClone(elkFiltered);

		return await elkToSVG().render(deepCopyElkFiltered);
	}
}
