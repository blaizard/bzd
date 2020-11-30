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

async function _next(data, cache, increment) {
	const device = await cache.get("sonos.device");
	let counter = device.playlist.length;

	while (counter--) {
		if (device.playlist.includes(device.track.uid)) {
			let index = (device.playlist.indexOf(device.track.uid) + increment) % device.playlist.length;
			index = (index < 0) ? (device.playlist.length + index) : index; 
			device.track.uid = device.playlist[index];
		}
		else {
			const uid = device.playlist[0] || null;
			device.track.uid = uid;
		}

		try {
			await device.instance.setAVTransportURI(device.track.uid);
			break;
		}
		catch (e) {
			if (counter == 0) {
				throw e;
			}
		}
	}
}

export default {
	cache: [
		{
			collection: "sonos.device",
			fetch: async (previous) => {
				let discovery = new Sonos.AsyncDeviceDiscovery();
				const instance = await discovery.discover();
				const description = await instance.deviceDescription();

				// Build the playlist
				const favorites = await instance.getFavorites();
				const playlist = favorites.items
					.filter((item) => {
						return "uri" in item;
					})
					.map((item) => {
						return item.uri;
					});

				let track = Object.assign(
					{
						uid: null,
						title: null,
						artist: null,
						art: null,
					},
					(previous || {}).track
				);

				instance.on("CurrentTrack", (currentTrack) => {
					if (currentTrack.title.startsWith("x-")) {
						track.uid = currentTrack.title;
					}
					else {
						track.title = currentTrack.title;
						track.artist = currentTrack.artist;
						track.art = currentTrack.albumArtURI;
					}
				});

				return {
					track: track,
					instance: instance,
					name: description.displayName + " - " + description.roomName,
					playlist: playlist,
				};
			},
			timeout: 60 * 1000 * 30,
		},
	],
	fetch: async (data, cache) => {
		const device = await cache.get("sonos.device");
		const state = await device.instance.getCurrentState();

		return {
			title: device.track.title,
			artist: device.track.artist,
			art: device.track.art,
			name: device.name,
			state: _getState(state),
		};
	},
	events: {
		async play(data, cache) {
			const device = await cache.get("sonos.device");
			await device.instance.play();
		},
		async pause(data, cache) {
			const device = await cache.get("sonos.device");
			await device.instance.pause();
		},
		async next(data, cache) {
			await _next(data, cache, 1);
		},
		async previous(data, cache) {
			await _next(data, cache, -1);
		},
	},
};
