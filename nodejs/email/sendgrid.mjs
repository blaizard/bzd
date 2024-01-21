import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";

import Email from "./email.mjs";

const Exception = ExceptionFactory("email", "sendgrid");
const Log = LogFactory("email", "sendgrid");

export default class Sendgrid extends Email {
	constructor(from, options) {
		super();

		Exception.assert("key" in options, "API key must be set.");
		this.key = options.key;
		this.from = from;

		Log.info("Using sendgrid for '{}'.", from);
	}

	/// Send an email
	async _sendImpl(toList, subject, data) {
		let content = [];
		if (data.html) {
			content.push({
				type: "text/html",
				value: data.html,
			});
		}
		if (data.text) {
			content.push({
				type: "text/plain",
				value: data.text,
			});
		}

		await HttpClient.request("https://api.sendgrid.com/v3/mail/send", {
			method: "post",
			authentication: {
				type: "bearer",
				token: this.key,
			},
			json: {
				personalizations: [
					{
						to: toList.map((to) => ({ email: to })),
						subject: subject,
					},
				],
				from: {
					email: this.from,
				},
				content: content,
			},
		});
	}
}
