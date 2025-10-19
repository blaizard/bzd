import Clock from "#bzd/nodejs/core/clock/clock.mjs";
import ntpClient from "ntp-client";

function getTime(server, port) {
	return new Promise((resolve, reject) => {
		ntpClient.getNetworkTime(server, port, (err, date) => {
			if (err) {
				reject(err);
				return;
			}
			resolve(date);
		});
	});
}

export default class ClockNTP extends Clock {
	constructor() {
		super();
		this.delta = undefined;
		this.interval = null;
	}

	async start() {
		await this._sync();

		// Sync every 12h.
		this.interval = setInterval(
			() => {
				this._sync().catch(() => {});
			},
			12 * 60 * 60 * 1000,
		);
	}

	async stop() {
		clearInterval(this.interval);
	}

	async _sync() {
		const now = await getTime("pool.ntp.org", 123);
		this.delta = now.getTime() - Date.now();
	}

	_getTime() {
		return Date.now() + this.delta;
	}
}
