import Stripe from "#bzd/nodejs/payment/stripe/webhook.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import testDataInvoicePaymentSucceeded1 from "#bzd/nodejs/payment/stripe/tests/invoice.payment_succeeded.json" assert { type: "json" };

const Exception = ExceptionFactory("test", "stripe");

class StripeForTest extends Stripe {
	_init() {
		this.stripe = null;
	}

	async getProduct(productId) {
		if (productId in this.products) {
			return this.products[productId];
		}
		const product = { metadata: { dummy: "test" } };
		this.products[productId] = Object.assign(
			{
				uid: productId,
			},
			product.metadata,
		);
		return this.products[productId];
	}
}

describe("Stripe", () => {
	describe("invoice.payment_succeeded", () => {
		it("1", async () => {
			let callbackCalled = 0;
			const stripe = new StripeForTest((uid, email, products, maybeSubscription) => {
				Exception.assert(uid == "pi_3OfeLCFcRmeweTPwA1RbxpqtS");
				Exception.assert(email == "dummy@test.com");
				Exception.assert(products.length == 1);
				Exception.assert(products[0].uid == "prod_POirZfdfEWvI02y");
				Exception.assert(maybeSubscription);
				Exception.assert(maybeSubscription.uid == "sub_1OfeLBFcRmSNT67AFXcK01ym");
				Exception.assert(maybeSubscription.timestampEndMs == 1709453565000);
				++callbackCalled;
			});
			await stripe.processInvoice(testDataInvoicePaymentSucceeded1);
			Exception.assert(callbackCalled == 1);
		});
	});
});
