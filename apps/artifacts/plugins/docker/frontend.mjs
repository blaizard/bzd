export default {
	name: "Docker",
	icon: "bzd-icon-link",
	form: [{ type: "Input", name: "docker.url", caption: "Docker URL" }],
	visualization: () => import("../table.vue")
};
