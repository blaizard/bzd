/// Translate a bdl architecture file into an elk json diagram.
///
/// See: https://eclipse.dev/elk/documentation/tooldevelopers/graphdatastructure/jsonformat.html
/// const graph = {
///   id: "root",
///   layoutOptions: { 'elk.algorithm': 'layered' },
///   children: [
///     { id: "n1", width: 30, height: 30 },
///     { id: "n2", width: 30, height: 30 },
///     { id: "n3", width: 30, height: 30 }
///   ],
///   edges: [
///     { id: "e1", sources: [ "n1" ], targets: [ "n2" ] },
///     { id: "e2", sources: [ "n1" ], targets: [ "n3" ] }
///   ]
/// };

export default class BdlToElk {
	constructor(bdl) {
		this.bdl = bdl;
	}

	*getAllComponents() {
		for (const context of this.bdl["contexts"]) {
			for (const [uid, component] of Object.entries(context["registry"])) {
				yield [uid, component];
			}
		}
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

	getComponentMembers(uid) {}

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

	getIOsFromUID(uid) {
		return this.bdl["ios"][uid] || {};
	}

	process() {
		let children = [];
		let edges = [];

		for (const [uid, component] of this.getAllComponents()) {
			const config = this.getParametersFromExpression(component.expression);
			const ios = this.getIOsFromUID(uid);
			let ports = [];
			for (const [name, io] of Object.entries(ios)) {
				const isSource = io.type == "source";
				ports.push({
					id: io.uid,
					width: 10,
					height: 10,
					labels: [
						{
							text: name,
						},
					],
					properties: {
						"port.side": isSource ? "EAST" : "WEST",
					},
				});
				if (isSource) {
					for (const sink of io.connections) {
						edges.push({
							id: io.uid + "-" + sink,
							sources: [io.uid],
							targets: [sink],
						});
					}
				}
			}

			children.push({
				id: uid,
				labels: [
					{
						text: uid,
					},
				],
				ports: ports,
			});
		}

		for (const [uid1, uid2] of this.getAllEdges()) {
			edges.push({
				id: uid1 + "-" + uid2,
				sources: [uid1],
				targets: [uid2],
			});
		}

		return {
			id: "root",
			layoutOptions: { "elk.algorithm": "layered" },
			children: children,
			edges: edges,
		};
	}
}
