export default {
	name: "Docker",
	icon: "bzd-icon-link",
	form: [
		{
			type: "Dropdown",
			name: "docker.type",
			caption: "Type",
			list: {
				v2: "Docker V2",
				gcr: "Google Container Registry"
			}
		},
		{ type: "Input", name: "docker.url", caption: "Registry URL", condition: { "docker.type": "values(v2)" } },
		{
			type: "Dropdown",
			name: "docker.service",
			editable: true,
			caption: "Service",
			list: ["gcr.io", "us.gcr.io", "eu.gcr.io", "asia.gcr.io"],
			condition: { "docker.type": "values(gcr)" }
		},
		{ type: "Textarea", name: "docker.key", caption: "JSON key", condition: { "docker.type": "values(gcr)" } }
	],
	visualization: () => import("../table.vue")
};
