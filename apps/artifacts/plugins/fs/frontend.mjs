export default {
	name: "File System",
	icon: "bzd-icon-link",
	form: [{ type: "Input", name: "fs.root", caption: "Full path of the root directory" }],
	view: () => import("../default.vue"),
};
