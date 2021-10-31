export default {
	type: "source",
	name: "Bzd Report",
	icon: "bzd-icon-link",
	visualization: ["coverage"],
	form: [
		{ type: "Input", name: "coverage.url", caption: "Coverage Report URL" },
		{
			type: "Dropdown",
			name: "coverage.authentication",
			caption: "Authentication Type",
			list: {
				none: "None",
				basic: "Basic",
			},
		},
		{ type: "Input", name: "coverage.user", caption: "Username", width: 0.5 },
		{ type: "Input", name: "coverage.token", caption: "Token", width: 0.5 },
	],
	timeout: 60 * 1000, // ms
};
