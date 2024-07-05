import loadScript from "#bzd/nodejs/core/script.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import { getQueryAsDict } from "#bzd/nodejs/utils/query.mjs";

const Exception = ExceptionFactory("authentication", "google");
const Log = LogFactory("authentication", "google");

async function triggerAuthentication(clientId, url) {
	await loadScript("https://accounts.google.com/gsi/client");

	return new Promise(async (resolve, reject) => {
		// Using https://developers.google.com/identity/oauth2/web/guides/use-code-model
		const client = google.accounts.oauth2.initCodeClient({
			client_id: clientId,
			scope: "https://www.googleapis.com/auth/userinfo.email",
			ux_mode: "redirect",
			redirect_uri: url + "/authentication/google",
		});

		client.requestCode();
	});
}

export default class Google {
	/// Create a client from Google identity
	///
	/// \param clientId The Google client ID, which can be created here: https://console.cloud.google.com/apis/credentials
	/// \param url The URL of the website to compute the redirect_uri.
	constructor(clientId, url) {
		this.clientId = clientId;
		this.url = url;
		this.rest = null;
	}

	installRest(rest) {
		rest.provide("google-authenticate", async () => await this.authenticate());
		this.rest = rest;
	}

	installRouter(router) {
		router.registerRouter({
			routes: [
				{
					path: "/authentication/google",
					handler: async () => {
						const query = getQueryAsDict();
						await this.rest.loginWithRest("post", "/auth/google", {
							code: query.code,
						});
					},
				},
			],
		});
	}

	// https://developers.google.com/identity/gsi/web/guides/overview
	async authenticate() {
		await triggerAuthentication(this.clientId, this.url);
	}
}
