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
	/// \param email The email linked with the payment.
	/// \param products The list of products associated with this payment. All products
	///                 have a 'uid' field which is a unique identifier for the product
	///                 at the payment provider side.
	/// \param maybeSubscription A subscription object, optional.
	/// \return true if it was processed, false otherwise.
	async callbackPayment(uid, email, products, maybeSubscription = null) {
		try {
			Exception.assert(typeof email === "string", "The email must be of type string: {}", email);
			Exception.assert(products.length > 0, "There must be at least one product: {}", products);
			return await this._callbackPayment(uid, email, products, maybeSubscription);
		} catch (e) {
			const error = Exception.fromError(e);
			error.print();
			throw new Exception(
				"Error with callbackPayment({}, {}, {}, {}): {}",
				uid,
				email,
				JSON.stringify(products),
				maybeSubscription,
				error.message,
			);
		}
	}
}
