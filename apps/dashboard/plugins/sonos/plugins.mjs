export default {
	source: {
		name: "Sonos",
		icon: "bzd-icon-sonos",
		visualization: [],
		form: [],
		timeout: 60 * 1000, // ms
		frontend: () => import("./source_frontend.vue"),
		backend: () => import("./source_backend.mjs"),
	},
};
