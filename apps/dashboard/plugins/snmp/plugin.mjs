export default {
	type: "source",
	name: "SNMP",
	visualization: ["system_monitor"],
	form: [
		{
			type: "Array",
			name: "snmp.array",
			caption: "OIDs",
			template: [
				{ type: "Input", caption: "Identifier", name: "id", width: 0.4 },
				{ type: "Input", caption: "OID", name: "oid", width: 0.4 },
				{ type: "Input", caption: "Time To Live", name: "ttl", width: 0.2 },
			],
			templateAdd: [
				{
					type: "Dropdown",
					name: "preset",
					caption: "Preset",
					list: {
						synology: "Synology",
					},
					width: 0.4,
				},
				{
					type: "Dropdown",
					name: "oid",
					caption: "OIDs",
					condition: (values) => values["preset"] == "synology",
					list: {
						"1.3.6.1.4.1.6574.1.5.1.0": "model",
						"1.3.6.1.2.1.25.2.3.1.5.31": "hdd",
					},
					width: 0.4,
				},
				{ type: "Button", content: "Add", action: "approve", width: 0.2, align: "bottom" },
			],
		},
	],
	timeout: 2000, // ms
};
