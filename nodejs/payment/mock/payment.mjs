import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { PaymentInterface } from "#bzd/nodejs/payment/payment.mjs";

const Exception = ExceptionFactory("payment", "mock");
const Log = LogFactory("payment", "mock");

export default class MockPayment extends PaymentInterface {
	constructor(callbackPayment, callbackCancelRecurrency, options) {
		super(callbackPayment, callbackCancelRecurrency);
		this.processed = new Set();
	}

	/// Trigger a payment for testing purposes only.
	async triggerPayment(uid, email, products, maybeRecurrency = null) {
		return await this.callbackPayment(uid, email, products, maybeRecurrency);
	}
}
