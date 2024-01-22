import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("payment");

export class Subscription {
	constructor(uid, timestampEndMs = null) {
		this.uid = uid;
		this.timestampEndMs = timestampEndMs;
	}
}

export class PaymentInterface {
	constructor(callbackPayment) {
		this._callbackPayment = callbackPayment;
	}

	/// callbackPayment(uid, email, products, maybeSubscription: Subscription = null)
	///
	/// \param uid The unique identifier corresponding to this payment.
	/// \param products The list of products associated with this payment. All products
	///                 have a 'uid' field which is a unique identifier for the product
	///                 at the payment provider side.
	/// \return true if it was processed, false otherwise.
	async callbackPayment(uid, email, products, maybeSubscription = null) {
		try {
			Exception.assert(products.length > 0, "There must be at least one product: {}", products);
			return await this._callbackPayment(uid, email, products, maybeSubscription);
		} catch (e) {
			Exception.fromError(e).print(
				"Error with callbackPayment({}, {}, {}, {})",
				uid,
				email,
				JSON.stringify(products),
				maybeSubscription,
			);
			throw e;
		}
	}
}
