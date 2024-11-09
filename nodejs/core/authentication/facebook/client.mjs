import loadScript from "#bzd/nodejs/core/script.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("authentication", "facebook");
const Log = LogFactory("authentication", "facebook");

async function triggerAuthentication(appId) {
	await loadScript("https://connect.facebook.net/en_US/sdk.js");

	return new Promise((resolve, reject) => {
		window.fbAsyncInit = function () {
			try {
				FB.init({
					appId: appId,
					xfbml: true,
					version: "v2.8",
				});

				FB.login(
					(response) => {
						if (response.authResponse) {
							resolve(response.authResponse);
						} else {
							reject("User cancelled login or did not fully authorize.");
						}
					},
					{
						scope: "email",
					},
				);
			} catch (e) {
				reject(e);
			}
		};
	});
}

export default class Facebook {
	/// Use the Facebook login
	///
	/// See: https://developers.facebook.com/docs/facebook-login/web
	///
	/// /!\: local test can be done with: http://localhost:8000 <- port seems important.
	constructor(appId) {
		this.appId = appId;
		this.rest = null;
	}

	installRest(rest) {
		rest.provide("facebook-authenticate", async () => await this.authenticate());
		this.rest = rest;
	}

	async authenticate() {
		const result = await triggerAuthentication(this.appId);
		await this.rest.loginWithRest("post", "/auth/facebook", {
			authResponse: result,
		});
	}
}
