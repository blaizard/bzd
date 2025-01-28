/// Translate a bdl architecture file into an elk json diagram.
///
/// See: https://eclipse.dev/elk/documentation/tooldevelopers/graphdatastructure/jsonformat.html
export default class BdlToElk {
	constructor() {
		this.tree = [];
		this.ios = {};
	}

	addTarget(target, bdl) {
		const targetTree = this.makeTree(bdl);
		this.tree.push({
			type: "target",
			fqn: target,
			expression: {
				name: target,
				fqn: target,
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
			return expression.value;
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

	includeFQN(fqn, options) {
		for (const target of options.filterOut) {
			if (fqn.startsWith(target)) {
				return false;
			}
		}
		return true;
	}

	processComponents(root, options, parent = null, level = 0) {
		let children = [];
		let edges = [];
		let deps = new Set();

		const addEdge = (edges, from, to, classes) => {
			if (this.includeFQN(from, options) && this.includeFQN(to, options)) {
				edges.push({
					id: from + "-" + to,
					sources: [from],
					targets: [to],
					classes: classes,
				});
			}
		};

		for (const component of root) {
			const fqn = component.fqn;
			const category = component.expression.category;

			if (category != "target" && !this.includeFQN(fqn, options)) {
				continue;
			}

			const name = (parent ? fqn.replace(parent, "") : fqn) + (category == "method" ? "(...)" : "");
			const config = this.getParametersFromExpression(component.expression);
			const ios = this.getIOsFromFQN(fqn);
			let ports = [];

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
				if (isSource) {
					for (const sink of io.connections) {
						addEdge(edges, io.uid, sink, ["io"]);
					}
				}
			}

			// Process nested
			const members = this.processComponents(component.members, options, /*parent*/ fqn, level + 1);
			const componentDeps = new Set([...members.deps, ...(component["deps"] || [])]);
			deps = new Set([...deps, ...componentDeps]);

			// Only dependencies between top-level and components elements are drawn.
			if (level == 1) {
				for (const depFQN of componentDeps) {
					// Only non-nested dependencies are drawn.
					if (!fqn.startsWith(depFQN)) {
						addEdge(edges, fqn, depFQN, ["dependency"]);
					}
				}
			}

			// Create the child
			children.push({
				id: fqn,
				labels: [
					{
						text: name,
					},
				],
				classes: ["level-" + level],
				children: members.children,
				edges: members.edges,
				ports: ports,
			});
		}

		return {
			children: children,
			edges: edges,
			deps: deps,
		};
	}

	process(options) {
		const { children, edges } = this.processComponents(this.tree, options);

		return {
			id: "root",
			children: children,
			edges: edges,
		};
	}
}
