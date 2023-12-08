import loadScript from "#bzd/nodejs/core/script.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";

const Exception = ExceptionFactory("authentication", "google");
const Log = LogFactory("authentication", "google");

async function triggerAuthentication(clientId) {
	await loadScript("https://accounts.google.com/gsi/client");

	return new Promise((resolve, reject) => {
		window.google.accounts.id.initialize({
			client_id: clientId,
			callback: (data) => {
				resolve(data);
			},
			error_callback: (err) => {
				reject(err);
			},
		});
		window.google.accounts.id.prompt((notification) => {
			Log.debug("Prompt notification: {}", JSON.stringify(notification));
			if (notification.isNotDisplayed() || notification.isSkippedMoment()) {
				// Needed to force google to reset its settings.
				document.cookie = "g_state=;path=/;expires=Thu, 01 Jan 1970 00:00:01 GMT";
				window.google.accounts.id.prompt();
			}
		});
	});
}

export default class Google {
	constructor(clientId) {
		this.clientId = clientId;
		this.api = null;
	}

	async installAPI(api) {
		api.provide("google-authenticate", async () => await this.authenticate());
		this.api = api;
	}

	// https://developers.google.com/identity/gsi/web/guides/overview
	async authenticate() {
		const result = await triggerAuthentication(this.clientId);
		const details = await this.api.request("post", "/auth/google", {
			idToken: result.credential,
		});
		await this.api.loginWithDetails(details);
	}
}
