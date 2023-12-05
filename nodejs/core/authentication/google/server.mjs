// https://developers.google.com/identity/gsi/web/guides/verify-google-id-token

import { OAuth2Client } from "google-auth-library";

export default class GoogleIdentityServer {
	constructor(clientId) {
		this.clientId = clientId;
	}

	async installAPI(api) {
		api.handle("post", "/auth/google", async function (inputs) {
			const client = new OAuth2Client();
			try {
				const ticket = await client.verifyIdToken({
					idToken: inputs.token_id,
					audience: this.clientId,
				});
				const payload = ticket.getPayload();
				console.log(payload);
			} catch (e) {
				throw this.httpError(401, "Unauthorized");
			}

			//const userid = payload.email;
		});
	}
}

const client = new OAuth2Client();
async function verify() {
	const ticket = await client.verifyIdToken({
		idToken: token,
		audience: CLIENT_ID, // Specify the CLIENT_ID of the app that accesses the backend
		// Or, if multiple clients access the backend:
		//[CLIENT_ID_1, CLIENT_ID_2, CLIENT_ID_3]
	});
	const payload = ticket.getPayload();
	const userid = payload["sub"];
	// If request specified a G Suite domain:
	// const domain = payload['hd'];
}
verify().catch(console.error);
