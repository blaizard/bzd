import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";
import HTTPClient from "#bzd/nodejs/core/http/client.mjs";

const Exception = ExceptionFactory("authentication", "facebook");
const Log = LogFactory("authentication", "facebook");

export default class FacebookIdentityServer {
	installRest(rest) {
		rest.handle("post", "/auth/facebook", async function (inputs) {
			let email = null;
			try {
				const response = await HTTPClient.request("https://graph.facebook.com/me", {
					query: {
						fields: "email",
						access_token: inputs.authResponse.accessToken,
					},
					expect: "json",
				});
				email = response.email;
			} catch (e) {
				throw this.httpError(401, "Unauthorized\n" + String(e));
			}

			Exception.assert(email, "The email cannot be null: {}", email);

			return await rest.loginWithUID(this, email, "facebook", /*persistent*/ false);
		});
	}
}
