import NodeMailer from "nodemailer";

import Email from "./email.mjs";

export default class Sendgrid extends Email {
	constructor(from, transportOptions) {
		super();

		this.from = from;
		this.transporter = NodeMailer.createTransport(transportOptions);
	}

	/// Send an email
	async _sendImpl(toList, subject, data) {
		await this.transporter.sendMail({
			from: this.from,
			to: toList.join(", "),
			subject: subject,
			text: data.format == "text" ? data.content : undefined,
			html: data.format == "html" ? data.content : undefined,
		});
	}
}
