import loadScript from "#bzd/nodejs/core/script.mjs";
import ExceptionFactory from "../../exception.mjs";
import LogFactory from "../../log.mjs";

const Exception = ExceptionFactory("authentication", "facebook");
const Log = LogFactory("authentication", "facebook");

async function triggerAuthentication(appId) {
	await loadScript("https://connect.facebook.net/en_US/sdk.js#xfbml=1&version=v20.0");

	return new Promise((resolve, reject) => {
		window.fbAsyncInit = () => {
			try {
				FB.init({
					appId: appId,
					xfbml: true,
					version: "v2.7",
				});
				FB.login(
					(response) => {
						if (response.authResponse) {
							resolve(response.authResponse);
						} else {
							reject("User cancelled login or did not fully authorize.");
						}
					},
					{ scope: "email" },
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
	constructor(appId) {
		this.appId = appId;
		this.rest = null;
	}

	async installRest(rest) {
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
