// https://developers.google.com/identity/gsi/web/guides/verify-google-id-token

import { OAuth2Client } from "google-auth-library";

import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";

const Exception = ExceptionFactory("authentication", "google");
const Log = LogFactory("authentication", "google");

export default class GoogleIdentityServer {
	constructor(clientId) {
		this.clientId = clientId;
	}

	async installRest(rest) {
		rest.handle("post", "/auth/google", async function (inputs) {
			const client = new OAuth2Client();
			let email = null;
			try {
				const ticket = await client.verifyIdToken({
					idToken: inputs.idToken,
					audience: this.clientId,
				});
				const payload = ticket.getPayload();
				if (!payload.email_verified) {
					throw this.httpError(401, "Unauthorized");
				}
				email = payload.email;
			} catch (e) {
				throw this.httpError(401, "Unauthorized");
			}

			Exception.assert(email, "The email cannot be null: {}", email);
			return await rest.loginWithUID(this, email, "google", /*persistent*/ false);
		});
	}
}
