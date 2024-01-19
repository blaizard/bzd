import NodeMailer from "#bzd/nodejs/email/nodemailer.mjs";
import SendGrid from "#bzd/nodejs/email/sendgrid.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";

const Exception = ExceptionFactory("email", "make-from-config");

function validateConfig(schema, config) {
	const validation = new Validation(schema);
	const result = validation.validate(config, { output: "result", all: true });
	Exception.assertResult(result);
}

export default async function makeFromConfig(config) {
	switch (config.type) {
		case "nodemailer":
			validateConfig(
				{
					type: "mandatory",
					from: "mandatory",
					options: "",
				},
				config,
			);
			return new NodeMailer(config.from, config.options);
		case "sendgrid":
			validateConfig(
				{
					type: "mandatory",
					from: "mandatory",
					options: "",
				},
				config,
			);
			return new SendGrid(config.from, config.options);
		default:
			Exception.unreachable("Unsupported email type: '{}'.", config.type);
	}
}
