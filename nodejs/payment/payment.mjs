export class Subscription {
	constructor(uid, end) {
		this.uid = uid;
		this.end = end;
	}
}

export class PaymentInterface {
	constructor(callbackPayment) {
		/// callbackPayment(uid, email, products, maybeSubscription: Subscription = null)
		///
		/// \param uid The unique identifier corresponding to this payment.
		/// \param products The list of products associated with this payment. All products
		///                 have a 'uid' field which is a unique identifier for the product
		///                 at the payment provider side.
		/// \return true if it was processed, false otherwise.
		this.callbackPayment = callbackPayment;
	}
}
