import Stripe from "#bzd/nodejs/payment/stripe/webhook.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import testDataInvoicePaymentSucceeded1 from "#bzd/nodejs/payment/stripe/tests/invoice.payment_succeeded.json" with { type: "json" };
import testDataCustomerSubscriptionDeleted1 from "#bzd/nodejs/payment/stripe/tests/customer.subscription.deleted.json" with { type: "json" };

const Exception = ExceptionFactory("test", "stripe");

class StripeForTest extends Stripe {
	_init() {
		this.stripe = null;
	}

	async getProduct(productId) {
		return {
			uid: productId,
			metadata: { dummy: "test" },
		};
	}

	async getCustomerEmail(customerId) {
		return customerId + "@dummy.com";
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

	describe("customer.subscription.deleted", () => {
		it("1", async () => {
			let callbackCalled = 0;
			const stripe = new StripeForTest(
				(uid, email, products, maybeSubscription) => {},
				(uid, email) => {
					Exception.assertEqual(uid, "sub_1Oj1KWFcRmSNTPwAe4FbkOMn");
					Exception.assertEqual(email, "cus_PY7ZxKt5nLDwU2@dummy.com");
					++callbackCalled;
				},
			);
			await stripe.processSubscriptionDeleted(testDataCustomerSubscriptionDeleted1);
			Exception.assert(callbackCalled == 1);
		});
	});
});
