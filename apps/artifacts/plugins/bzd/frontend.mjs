export default {
	name: "Bzd",
	icon: "bzd-icon-link",
	form: [
		{ type: "Input", name: "bzd.path", caption: "The path where to save the data.", mandatory: true },
		{ type: "Input", name: "bzd.server.port", caption: "The port on which the server listen to.", mandatory: true },
	],
	view: function () {
		switch (this.pathList.length) {
			case 2:
				return import("#bzd/apps/artifacts/plugins/bzd/view_node.vue");
			case 3:
				return import("#bzd/apps/artifacts/plugins/bzd/view_category.vue");
		}
		return import("#bzd/apps/artifacts/plugins/default.vue");
	},
};
