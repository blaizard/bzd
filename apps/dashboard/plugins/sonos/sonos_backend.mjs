import Sonos from "sonos";

function _getState(state) {
	if (state == "playing" || state == "transitioning") {
		return "play";
	}
	if (state == "stopped") {
		return "pause";
	}
	console.log(state);
	return "unknown";
}

export default {
	cache: [
		{
			collection: "sonos.device",
			fetch: async () => {
				let discovery = new Sonos.AsyncDeviceDiscovery();
				return await discovery.discover();
			},
			timeout: 60 * 1000 * 30,
		},
	],
	fetch: async (data, cache) => {
		const device = await cache.get("sonos.device");
		const track = await device.currentTrack();
		const state = await device.getCurrentState();
		return {
			title: track.title,
			artist: track.artist,
			state: _getState(state) // transitioning. playing
		};
	},
	events: {
		async play(data, cache) {
			const device = await cache.get("sonos.device");
			await device.play();
		},
		async pause(data, cache) {
			const device = await cache.get("sonos.device");
			await device.pause();
		}
	}
};
