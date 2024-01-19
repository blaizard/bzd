import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Template from "#bzd/nodejs/core/template.mjs";
import TemplateResetPassword from "#bzd/apps/accounts/backend/email/reset_password.mjs";

const Exception = ExceptionFactory("emails");
const Log = LogFactory("emails");

/// Wrapper class around a user.
export default class EmailsManager {
	constructor(mail) {
		this.mail = mail;
		this.templates = {
			resetPassword: new Template(TemplateResetPassword),
		};
	}

	async _send(email, title, template, attributes) {
		const content = template.process(attributes);
		await this.mail.send(email, title, {
			format: "html",
			content: content,
		});
	}

	async sendResetPassword(email, attributes) {
		await this._send(email, "Password Reset", this.templates.resetPassword, attributes);
	}
}
