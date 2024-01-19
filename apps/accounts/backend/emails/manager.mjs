import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import NodeMailer from "#bzd/nodejs/email/nodemailer.mjs";
import Template from "#bzd/nodejs/core/template.mjs";
import TemplateResetPassword from "#bzd/apps/accounts/backend/emails/reset_password.mjs";

const Exception = ExceptionFactory("emails");
const Log = LogFactory("emails");

/// Wrapper class around a user.
export default class EmailsManager {
	constructor(from, options, testMode = false) {
		this.mail = new NodeMailer(from, options);
		this.testMode = testMode;
		this.templates = {
			resetPassword: new Template(TemplateResetPassword),
		};
	}

	async _send(email, title, template, attributes) {
		if (this.testMode) {
			console.log("==============");
			console.log("to: " + email);
			console.log("title: " + title);
			for (const [key, value] of Object.entries(attributes)) {
				console.log(key + ": " + value);
			}
			console.log("==============");
		} else {
			const content = template.process(attributes);
			await this.mail.send(email, title, {
				format: "html",
				content: content,
			});
		}
	}

	async sendResetPassword(email, attributes) {
		await this._send(email, "Password Reset", this.templates.resetPassword, attributes);
	}
}
