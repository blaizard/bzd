// https://developers.google.com/identity/gsi/web/guides/verify-google-id-token

import { OAuth2Client } from "google-auth-library";

import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";

const Exception = ExceptionFactory("authentication", "google");
const Log = LogFactory("authentication", "google");

export default class GoogleIdentityServer {
	constructor(clientId, clientSecret, url) {
		this.clientId = clientId;
		this.clientSecret = clientSecret;
		this.url = url;
	}

	installRest(rest) {
		const self = this;
		rest.handle("post", "/auth/google", async function (inputs) {
			const client = new OAuth2Client(self.clientId, self.clientSecret, this.url + "/authentication/google");
			let email = null;
			try {
				// Exchange code for tokens
				const { tokens } = await client.getToken(inputs.code);
				const ticket = await client.verifyIdToken({
					idToken: tokens.id_token,
					audience: self.clientId,
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
