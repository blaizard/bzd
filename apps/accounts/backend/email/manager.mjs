import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import resetPassword from "#bzd/apps/accounts/backend/email/reset_password/reset_password.mjs";

const Exception = ExceptionFactory("emails");
const Log = LogFactory("emails");

/// Wrapper class around a user.
export default class EmailsManager {
	constructor(mail) {
		this.mail = mail;
	}

	async sendResetPassword(email, attributes) {
		await this.mail.send(email, "Password Reset", resetPassword(attributes));
	}
}
