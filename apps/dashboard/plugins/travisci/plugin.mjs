export default {
	type: "source",
	name: "Travis CI",
	icon: "bzd-icon-travisci",
	visualization: ["continuous_integration"],
	form: [
		{
			type: "Dropdown",
			name: "travisci.endpoint",
			caption: "Endpoint",
			validation: "mandatory",
			list: ["travis-ci.org", "travis-ci.com"]
		},
		{
			type: "Input",
			name: "travisci.repository",
			caption: "Repository",
			validation: "mandatory",
			placeholder: "For example, blaizard/cpp-async",
			width: 0.5
		},
		{ type: "Input", name: "travisci.token", caption: "Token", width: 0.5 }
	],
	timeout: 60 * 1000 // ms
};
