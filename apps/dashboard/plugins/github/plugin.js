export default {
	type: "source",
	name: "Github CI",
	icon: "bzd-icon-github",
	visualization: ["continuous_integration"],
	form: [
		{
			type: "Input",
			name: "username",
			caption: "Username",
			placeholder: "For example, blaizard",
			width: 0.5,
		},
		{
			type: "Input",
			name: "repository",
			caption: "Repository",
			placeholder: "For example, cpp-async",
			width: 0.5,
		},
		{
			type: "Input",
			name: "workflowId",
			caption: "Workflow ID (optional)",
			placeholder: "For example, ci.yml",
			width: 0.5,
		},
		{
			type: "Input",
			name: "token",
			caption: "Token (optional)",
			width: 0.5,
		},
	],
	timeout: 60 * 1000, // ms
};
