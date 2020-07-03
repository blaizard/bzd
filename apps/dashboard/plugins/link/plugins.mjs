

export default {
	visualization: {
		name: "Link",
		icon: "bzd-icon-link",
		form: [
			{ type: "Input", name: "link.url", caption: "URL", placeholder: "https://www.google.com" },
		],
		frontend: () => import("./visualization.vue")
	}
};
