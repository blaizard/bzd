import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import configResetPassword from "#bzd/apps/accounts/backend/email/reset_password/reset_password.mjs";
import configWelcome from "#bzd/apps/accounts/backend/email/welcome/welcome.mjs";

const Exception = ExceptionFactory("emails");
const Log = LogFactory("emails");

/// Wrapper class around a user.
export default class EmailsManager {
	constructor(mail) {
		this.mail = mail;
	}

	async sendResetPassword(email, attributes) {
		await this.mail.send(email, "Password Reset", configResetPassword(attributes));
	}

	async sendWelcome(email, attributes) {
		await this.mail.send(email, "Welcome", configWelcome(attributes));
	}
}
