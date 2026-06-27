/// Translate a bdl architecture file into an elk json diagram.
///
/// See: https://eclipse.dev/elk/documentation/tooldevelopers/graphdatastructure/jsonformat.html
export default class BdlToElk {
	constructor() {
		this.tree = [];
		this.ios = {};
	}

	addTarget(bdl) {
		const targetTree = this.makeTree(bdl);
		this.tree.push({
			type: "target",
			fqn: bdl.target,
			expression: {
				name: bdl.target,
				fqn: bdl.target,
				category: "target",
			},
			members: targetTree,
		});
		Object.assign(this.ios, bdl["ios"]);
	}

	/// Create a list of all top level elements and their nested members.
	///
	/// It adds the following information to the elements:
	/// - fqn: The FQN of the element, note that it is not unique, the same function can be called multiple times.
	/// - type: The type of the element.
	/// - members: The members of the elements.
	makeTree(bdl) {
		let list = [];
		const addElementToMap = (element, type) => {
			const fqn = element.expression.fqn || element.expression.symbol;
			list.push(
				Object.assign({}, element, {
					type: type,
					fqn: fqn,
					members: [],
				}),
			);
		};

		for (const context of bdl["contexts"]) {
			for (const workload of context["workloads"] || []) {
				addElementToMap(workload, "workload");
			}
			for (const service of context["services"] || []) {
				addElementToMap(service, "service");
			}
			for (const component of Object.values(context["registry"])) {
				addElementToMap(component, "registry");
			}
		}

		let topLevel = [];
		for (const maybeNestedElement of list) {
			let isTopLevel = true;
			for (const element of list) {
				if (maybeNestedElement == element) {
					continue;
				}
				if (maybeNestedElement.fqn.startsWith(element.fqn)) {
					element.members.push(maybeNestedElement);
					isTopLevel = false;
				}
			}
			if (isTopLevel) {
				topLevel.push(maybeNestedElement);
			}
		}

		return topLevel;
	}

	getExpressionValue(expression) {
		if ("value" in expression) {
			const value = expression.value;
			if (value.constructor == Object) {
				if (value.type == "enum") {
					return value.fqn;
				}
			}
			return value;
		}
		let output = expression["symbol"] || "<unknown>";
		if ("parameters" in expression) {
			const values = expression.parameters.map((param) => this.getExpressionValue(param)).join(", ");
			output += "(" + values + ")";
		}
		return output;
	}

	getParametersFromExpression(expression) {
		let parameters = [];
		for (const parameter of expression.parameters || []) {
			parameters.push(parameter.name + ": " + this.getExpressionValue(parameter));
		}
		return parameters;
	}

	getIOsFromFQN(fqn) {
		return this.ios[fqn] || {};
	}

	processComponents(root, parent = null, level = 0) {
		let children = [];
		let edges = [];
		let deps = new Set();
		// Set of target ID known at this level.
		let targets = new Set();

		const addEdge = (edges, from, to, classes) => {
			edges.push({
				id: from + "-" + to,
				sources: [from],
				targets: [to],
				classes: classes,
			});
		};

		const isEdgeKnown = (edge, targets) => {
			for (const edgeId of [...edge.sources, ...edge.targets]) {
				if (!targets.has(edgeId)) {
					return false;
				}
			}
			return true;
		};

		for (const component of root) {
			const fqn = component.fqn;
			const category = component.expression.category;
			const name = level == 2 ? fqn.replace(parent, "") : fqn;
			const displayName = name + (category == "method" ? "(...)" : "");
			const config = this.getParametersFromExpression(component.expression);
			const ios = this.getIOsFromFQN(fqn);
			let ports = [];

			targets.add(fqn);

			// Create the ports
			for (const [ioName, io] of Object.entries(ios)) {
				const isSource = io.type == "source";
				ports.push({
					id: io.uid,
					width: 10,
					height: 10,
					labels: [
						{
							text: ioName,
						},
					],
					properties: {
						"port.side": isSource ? "EAST" : "WEST",
					},
				});
				targets.add(io.uid);
				if (isSource) {
					for (const sink of io.connections) {
						addEdge(edges, io.uid, sink, ["io"]);
					}
				}
			}

			// Process nested
			const members = this.processComponents(component.members, /*parent*/ fqn, level + 1);
			const componentDeps = new Set([...members.deps, ...(component["deps"] || [])]);
			deps = new Set([...deps, ...componentDeps]);
			targets = new Set([...targets, ...members.targets]);
			edges = [...edges, ...members.edges];

			// Only dependencies between top-level and components elements are drawn.
			if (level == 1) {
				for (const depFQN of componentDeps) {
					// Only non-nested dependencies are drawn.
					if (!fqn.startsWith(depFQN)) {
						addEdge(edges, fqn, depFQN, ["dependency"]);
					}
				}
			}

			// Separate edges from the known ones only and unknown ones at this level.
			let edgesChildren = [];
			edges = edges.filter((edge) => {
				if (isEdgeKnown(edge, members.targets)) {
					edgesChildren.push(edge);
					return false;
				}
				return true;
			});

			// Create the child
			children.push({
				id: fqn,
				tooltip: "config:\n" + config.map((l) => "- " + l).join("\n"),
				labels: [
					{
						text: displayName,
					},
				],
				classes: ["level-" + level],
				children: members.children,
				edges: edgesChildren,
				ports: ports,
			});
		}

		return {
			children: children,
			edges: edges,
			deps: deps,
			targets: targets,
		};
	}

	process() {
		const { children, edges } = this.processComponents(this.tree);

		return {
			id: "root",
			children: children,
			edges: edges,
		};
	}
}
