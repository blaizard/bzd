export default {
	name: "WebDAV",
	icon: "bzd-icon-link",
	form: [
		{ type: "Input", name: "webdav.url", caption: "URL", mandatory: true },
		{ type: "Input", name: "webdav.username", caption: "Username for the path", mandatory: true },
		{ type: "Input", name: "webdav.password", caption: "Password for the path" },
	],
	view: () => import("../default.vue"),
};
