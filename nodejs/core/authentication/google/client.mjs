import loadScript from "#bzd/nodejs/core/script.mjs";

export default class Google {
	constructor(clientId) {
		this.clientId = clientId;
	}

	// Using https://developers.google.com/identity/oauth2/web/guides/use-token-model
	async authenticate() {
		await loadScript("https://accounts.google.com/gsi/client");

		return new Promise((resolve, reject) => {
			const client = google.accounts.oauth2.initTokenClient({
				client_id: this.clientId,
				scope: "https://www.googleapis.com/auth/cloud-platform.read-only",
				callback: (tokenResponse) => {
					resolve(tokenResponse);
				},
				error_callback: (err) => {
					reject(err);
				},
			});

			client.requestAccessToken();
		});
	}
}
