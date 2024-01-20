import LogFactory from "#bzd/nodejs/core/log.mjs";
import Email from "./email.mjs";

const Log = LogFactory("email", "stub");

export default class StubEmail extends Email {
	constructor(from, options) {
		super();

		Log.info("Using email stub for '{}'.", from);
	}

	async _sendImpl(toList, subject, data) {
		Log.info("Sending email '{}' to {}", subject, toList.join(", "));
	}
}
