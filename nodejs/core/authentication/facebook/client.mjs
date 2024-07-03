import loadScript from "#bzd/nodejs/core/script.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Cookie from "#bzd/nodejs/core/cookie.mjs";

const Exception = ExceptionFactory("authentication", "facebook");
const Log = LogFactory("authentication", "facebook");

async function triggerAuthentication(appId) {
	return new Promise((resolve, reject) => {
		try {
			FB.getLoginStatus((response) => {
				if (response.status === "connected") {
					resolve(response.authResponse);
				} else {
					// This seems to solve some weird issues that I faced, see:
					// https://stackoverflow.com/questions/34762428/fb-getloginstatus-is-returning-status-unknown-even-when-the-user-is-logged-in
					for (const prefix of ["fblo", "fbm", "fbsr"]) {
						Cookie.remove(prefix + "_" + appId);
					}
					FB.login(
						(response) => {
							if (response.status === "connected") {
								resolve(response.authResponse);
							} else {
								reject("User cancelled login or did not fully authorize.");
							}
						},
						{
							scope: "email",
						},
					);
				}
			}, true);
		} catch (e) {
			reject(e);
		}
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

		await loadScript("https://connect.facebook.net/en_US/sdk.js#version=v20.0");
		window.fbAsyncInit = () => {
			FB.init({
				appId: this.appId,
				status: true,
				version: "v2.8",
			});
		};
	}

	async authenticate() {
		const result = await triggerAuthentication(this.appId);
		await this.rest.loginWithRest("post", "/auth/facebook", {
			authResponse: result,
		});
	}
}
