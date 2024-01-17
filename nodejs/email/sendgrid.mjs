import ExceptionFactory from "../core/exception.mjs";
import HttpClient from "../core/http/client.mjs";

import Email from "./email.mjs";

const Exception = ExceptionFactory("email", "sendgrid");

export default class Sendgrid extends Email {
	constructor(options) {
		super();

		Exception.assert("key" in options, "API key must be set.");
		Exception.assert("from" in options, "From address must be set.");
		this.key = options.key;
		this.from = options.from;
	}

	/// Send an email
	async _sendImpl(toList, subject, data) {
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
				content: [
					{
						type: data.format == "text" ? "text/plain" : "text/html",
						value: data.content,
					},
				],
			},
		});
	}
}
