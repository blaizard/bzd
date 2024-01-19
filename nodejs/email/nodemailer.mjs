import NodeMailer from "nodemailer";
import LogFactory from "#bzd/nodejs/core/log.mjs";

import Email from "./email.mjs";

const Log = LogFactory("email", "nodemailer");

export default class NodeMailerEmail extends Email {
	constructor(from, transportOptions) {
		super();

		this.from = from;
		this.transporter = NodeMailer.createTransport(transportOptions);

		Log.info("Using nodemailer for {} with host: {}.", from, transportOptions.host);
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
