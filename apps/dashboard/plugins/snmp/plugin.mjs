export default {
	type: "source",
	name: "SNMP",
	visualization: ["system_monitor"],
	form: [
		{
			type: "Input",
			caption: "Host",
			name: "snmp.host",
			validation: "mandatory",
			width: 0.5,
		},
		{
			type: "Input",
			caption: "Community",
			name: "snmp.community",
			validation: "mandatory",
			placeholder: "public",
			width: 0.5,
		},
		{
			type: "Array",
			name: "snmp.array",
			caption: "OIDs",
			template: [
				{ type: "Input", caption: "Identifier", name: "id", width: 0.2 },
				{ type: "Input", caption: "OID", name: "oid", width: 0.2 },
				{ type: "Input", caption: "Time To Live", name: "ttl", width: 0.2, post: "s" },
				{
					type: "Array",
					caption: "Operations",
					name: "ops",
					width: 0.4,
					template: [
						{ type: "Dropdown", name: "type", list: { mul: "*", div: "/", add: "+", sub: "-" }, width: 0.1 },
						{ type: "Input", name: "value", width: 0.9 },
					],
				},
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
					// Check available disks: snmpwalk -v 2c -c public 192.168.1.200 1.3.6.1.2.1.25.2.3.1.3
					list: {
						"name:1.3.6.1.4.1.6574.1.5.1.0:600": "Model Name",
						"tempature:1.3.6.1.4.1.6574.1.2.0:10": "System Temperature (C)",
						"cpu:1.3.6.1.4.1.2021.11.9.0:10:add:1.3.6.1.4.1.2021.11.10.0:div:100": "CPU Load (%)",
						"memory.total:1.3.6.1.4.1.2021.4.5.0:600": "Total Memory (bytes)",
						"memory.free:1.3.6.1.4.1.2021.4.6.0:10:add:.1.3.6.1.4.1.2021.4.15.0": "Free Memory (bytes)",
						"temperature:1.3.6.1.4.1.6574.2.1.1.6.0:10": "Disk 1 Temperature (C)",
						"temperature:1.3.6.1.4.1.6574.2.1.1.6.1:10": "Disk 2 Temperature (C)",
						"ups.charge:1.3.6.1.4.1.6574.4.3.1.1.0:60:div:100": "UPS Battery Charge (%)",
						"ups.load:1.3.6.1.4.1.6574.4.2.12.1.0:60": "UPS Battery Load",
						"disk.total:1.3.6.1.2.1.25.2.3.1.5.51:600:mul:1.3.6.1.2.1.25.2.3.1.4.51": "/volume1 Total Space (bytes)",
						"disk.used:1.3.6.1.2.1.25.2.3.1.6.51:60:mul:1.3.6.1.2.1.25.2.3.1.4.51": "/volume1 Used Space (bytes)",
					},
					width: 0.4,
				},
				{ type: "Button", content: "Add", action: "approve", width: 0.2, align: "bottom" },
			],
			toValues: (values) => {
				const splittedValues = (values["oid"] || "").split(":");
				let operations = [];
				for (let i = 3; i < splittedValues.length; i += 2) {
					operations.push({
						type: splittedValues[i],
						value: splittedValues[i + 1],
					});
				}
				return {
					id: splittedValues[0],
					oid: splittedValues[1],
					ttl: splittedValues[2],
					ops: operations,
				};
			},
		},
	],
	timeout: 2000, // ms
};
