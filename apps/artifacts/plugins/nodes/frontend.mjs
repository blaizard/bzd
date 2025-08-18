export default {
	name: "Nodes",
	icon: "bzd-icon-link",
	form: [],
	view: function () {
		switch (this.pathList.length) {
			case 1:
				return import("#bzd/apps/artifacts/plugins/default.vue");
			default:
				return import("#bzd/apps/artifacts/plugins/nodes/view_category.vue");
		}
	},
};
