import loadScript from "#bzd/nodejs/core/script.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import Cookie from "#bzd/nodejs/core/cookie.mjs";

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
			use_fedcm_for_prompt: true,
		});
		// Note, with Safari or Edge it doesn't seem to work, see https://dev.to/intricatecloud/passwordless-sign-in-with-google-one-tap-for-web-4l51
		// on "Limitations of the One Tap Sign-In button"
		window.google.accounts.id.prompt((notification) => {
			Log.info("Google prompt notification: {}", JSON.stringify(notification));
			if (notification.isSkippedMoment()) {
				// Needed to force google to reset its settings.
				Cookie.remove("g_state");
				window.google.accounts.id.prompt();
			}
		});
	});
}

export default class Google {
	/// Create a client from Google identity
	///
	/// \param clientId The Google client ID, which can be created here: https://console.cloud.google.com/apis/credentials
	constructor(clientId) {
		this.clientId = clientId;
		this.rest = null;
	}

	async installRest(rest) {
		rest.provide("google-authenticate", async () => await this.authenticate());
		this.rest = rest;
	}

	// https://developers.google.com/identity/gsi/web/guides/overview
	async authenticate() {
		const result = await triggerAuthentication(this.clientId);
		await this.rest.loginWithRest("post", "/auth/google", {
			idToken: result.credential,
		});
	}
}
