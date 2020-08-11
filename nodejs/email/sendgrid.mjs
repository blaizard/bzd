import Email from "./email.mjs";
import ExceptionFactory from "../core/exception.mjs";
import Fetch from "../core/fetch.mjs";

const Exception = ExceptionFactory("email", "sendgrid");

/**
 * Email module
 */
export default class Sendgrid extends Email {
	constructor(options) {
		super();

		Exception.assert("key" in options, "API key must be sent.");
		Exception.assert("from" in options, "From address must be sent.");
		this.key = options.key;
		this.from = options.from;
	}

	/**
	 * Send an email
	 */
	async _sendImpl(toList, subject, data) {
		await Fetch.request("https://api.sendgrid.com/v3/mail/send", {
			method: "post",
			authentication: {
				type: "bearer",
				token: this.key
			},
			data: {
				personalizations: [
					{
						to: toList.map((to) => ({ email: to })),
						subject: subject
					}
				],
				from: {
					email: this.from
				},
				content: [
					{
						type: data.format == "text" ? "text/plain" : "text/html",
						value: data.content
					}
				]
			}
		});
	}
}
