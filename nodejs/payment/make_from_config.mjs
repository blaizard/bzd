import PaymentMock from "#bzd/nodejs/payment/mock/payment.mjs";
import PaymentStripe from "#bzd/nodejs/payment/stripe/webhook.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";

const Exception = ExceptionFactory("payment", "make-from-config");

function validateConfig(schema, config) {
	const validation = new Validation(schema);
	const result = validation.validate(config, { output: "result", all: true });
	Exception.assertResult(result);
}

export default async function makeFromConfig(callbackPayment, config) {
	validateConfig(
		{
			type: "mandatory",
			options: "",
		},
		config,
	);

	switch (config.type) {
		case "stripe":
			return new PaymentStripe(callbackPayment, config.options);
		case "mock":
			return new PaymentMock(callbackPayment, config.options);
		default:
			Exception.unreachable("Unsupported payment type: '{}'.", config.type);
	}
}
