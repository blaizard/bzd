export default {
	type: "source",
	name: "SNMP",
	visualization: ["system_monitor"],
	form: [
		{
			type: "Dropdown",
			name: "snmp.preset",
			caption: "Preset",
			list: ["Synology"],
		},
		{
			type: "Dropdown",
			name: "snmp.oids",
			caption: "OIDs",
			list: {
				"1.3.6.1.4.1.6574.1.5.1.0": "model",
				"1.3.6.1.2.1.25.2.3.1.5.31": "hdd",
			},
			multi: true,
		},
	],
	timeout: 2000, // ms
};
