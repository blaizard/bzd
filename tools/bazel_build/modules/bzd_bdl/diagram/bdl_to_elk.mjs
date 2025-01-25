/// Translate a bdl architecture file into an elk json diagram.
///
/// See: https://eclipse.dev/elk/documentation/tooldevelopers/graphdatastructure/jsonformat.html
export default class BdlToElk {
	constructor(bdl, target) {
		this.bdl = bdl;
		this.target = target;
		this.tree = this.makeTree();
	}

	/// Create a list of all top level elements and their nested members.
	///
	/// It adds the following information to the elements:
	/// - fqn: The FQN of the element, note that it is not unique, the same function can be called multiple times.
	/// - type: The type of the element.
	/// - members: The members of the elements.
	makeTree() {
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

		for (const context of this.bdl["contexts"]) {
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

	*getAllEdges() {
		// All dependencies between components
		for (const context of this.bdl["contexts"]) {
			for (const [uid, component] of Object.entries(context["registry"])) {
				for (const dep of component["deps"]) {
					yield [uid, dep];
				}
			}
		}
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
		return this.bdl["ios"][fqn] || {};
	}

	*getComponents(root, options) {
		for (const component of root) {
			if (options.onlyApplication && component.fqn.startsWith(this.target)) {
				continue;
			}
			yield component;
		}
	}

	includeFQN(fqn, options) {
		if (options.onlyApplication && fqn.startsWith(this.target)) {
			return false;
		}
		return true;
	}

	processComponents(root, options, parent = null) {
		let children = [];
		let edges = [];
		let deps = new Set();

		const addEdge = (from, to) => {
			if (this.includeFQN(from, options) && this.includeFQN(to, options)) {
				edges.push({
					id: from + "-" + to,
					sources: [from],
					targets: [to],
				});
			}
		};

		for (const component of root) {
			const fqn = component.fqn;
			if (!this.includeFQN(fqn, options)) {
				continue;
			}

			const name = parent ? fqn.replace(parent, "") : fqn;
			const config = this.getParametersFromExpression(component.expression);
			const ios = this.getIOsFromFQN(fqn);
			let ports = [];
			let componentDeps = new Set();

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
						addEdge(io.uid, sink);
					}
				}
			}

			// Process nested
			const members = this.processComponents(component.members, options, /*parent*/ fqn);
			componentDeps = new Set([...componentDeps, ...members.deps]);

			// Create the child
			children.push({
				id: fqn,
				labels: [
					{
						text: name,
					},
				],
				children: members.children,
				ports: ports,
			});

			// Create the edges
			for (const depFQN of component["deps"] || []) {
				componentDeps.add(depFQN);
			}

			// Only dependencies between top-level elements are drawn.
			if (parent === null) {
				for (const depFQN of componentDeps) {
					// Only non-nested dependencies are drawn.
					if (!fqn.startsWith(depFQN)) {
						addEdge(fqn, depFQN);
					}
				}
			}

			// Propagate the deps.
			deps = new Set([...deps, ...componentDeps]);
		}

		return {
			children: children,
			edges: edges,
			deps: deps,
		};
	}

	process(onlyApplication) {
		let { children, edges } = this.processComponents(this.tree, {
			onlyApplication: onlyApplication,
		});

		return {
			id: "root",
			layoutOptions: { "elk.algorithm": "layered" },
			children: children,
			edges: edges,
		};
	}
}
