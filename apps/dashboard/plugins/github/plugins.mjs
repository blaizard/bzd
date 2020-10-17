export default {
	source: {
		name: "Github CI",
		icon: "bzd-icon-github",
		visualization: ["continuous-integration"],
		form: [
			{
				type: "Input",
				name: "github.username",
				caption: "Username",
				placeholder: "For example, blaizard",
				width: 0.5,
			},
			{
				type: "Input",
				name: "github.repository",
				caption: "Repository",
				placeholder: "For example, cpp-async",
				width: 0.5,
			},
			{
				type: "Input",
				name: "github.workflowid",
				caption: "Workflow ID (optional)",
				placeholder: "For example, ci.yml",
				width: 0.5,
			},
			{
				type: "Input",
				name: "github.token",
				caption: "Token (optional)",
				width: 0.5,
			},
		],
		timeout: 60 * 1000, // ms
		frontend: () => import("./source_frontend.vue"),
		backend: () => import("./source_backend.mjs"),
	},
};
