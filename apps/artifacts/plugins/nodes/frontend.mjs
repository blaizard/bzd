export default {
	name: "Nodes",
	icon: "bzd-icon-link",
	form: [
		{ type: "Input", name: "nodes.path", caption: "The path where to save the data.", mandatory: true },
		{ type: "Input", name: "nodes.server.port", caption: "The port on which the server listen to.", mandatory: true },
	],
	view: function () {
		switch (this.pathList.length) {
			case 1:
				return import("#bzd/apps/artifacts/plugins/default.vue");
			case 2:
				return import("#bzd/apps/artifacts/plugins/nodes/view_node.vue");
			default:
				return import("#bzd/apps/artifacts/plugins/nodes/view_category.vue");
		}
	},
};
