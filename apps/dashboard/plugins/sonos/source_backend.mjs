import { AsyncDeviceDiscovery } from "sonos";

/*
function runDeviceDiscovery()
{
	return new Promise((resolve, reject) => {
		DeviceDiscovery().once("DeviceAvailable", (device) => {
			resolve(device.host);
		});
	});
}
*/
export default {
	cache: [
		{
			collection: "sonos.device",
			fetch: async () => {
				let discovery = new AsyncDeviceDiscovery();
				return await discovery.discover();
			},
			timeout: 60 * 1000,
		},
	],
	fetch: async (data, cache) => {
		const host = await cache.get("sonos.device");
		console.log("host", host);
		return {
			host: host,
		};
	},
};
