import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Stripe from "stripe";

const Exception = ExceptionFactory("payment", "stripe");
const Log = LogFactory("payment", "stripe");

class Subscription {
	constructor(reference, end) {
		this.reference = reference;
		this.end = end;
	}
}

export default class StripePaymentWebhook {
	constructor(options) {
		this.options = Object.assign(
			{
				/// You API secret Key.
				secretKey: null,
				/// The endpoint secret.
				secretEndpoint: null,
				/// callbackPayment(ref, email, products, maybeSubscription: Subscription = null)
				callbackPayment: null,
			},
			options,
		);
		Exception.assert(this.options.secretKey, "'secretKey' is missing.");
		Exception.assert(this.options.secretEndpoint, "'secretEndpoint' is missing.");
		this.stripe = Stripe(this.options.secretKey);
		// Map containing the matching between product identifier and the actual product.
		this.products = {};
		// Contain all already processed payments.
		this.processed = new Set();
	}

	async installAPI(api) {
		Log.debug("Installing Stripe webhook API.");

		const self = this;
		api.handle("post", "/stripe", async function (inputs) {
			// See: https://stripe.com/docs/payments/handling-payment-events
			const signature = this.getHeader("stripe-signature");

			// Read the event.
			let event;
			try {
				event = self.stripe.webhooks.constructEvent(inputs.raw, signature, self.options.secretEndpoint);
			} catch (e) {
				Exception.assertPrecondition(false, "Webhook error: '{}'.", e.message);
			}

			// Handle the events.
			switch (event.type) {
				case "checkout.session.completed":
					await self.processCheckoutSession(event.data.object);
					break;

				case "invoice.payment_succeeded":
					await self.processInvoice(event.data.object);
					break;

				default:
				// Ignore.
			}
		});
	}

	async getProduct(productId) {
		if (productId in this.products) {
			return this.products[productId];
		}
		const product = await this.stripe.products.retrieve(productId);
		this.products[productId] = product;
		return product;
	}

	async processCheckoutSession(checkoutSession) {
		// Ignore if there is an invoice, it will be handled by the invoice instead.
		if (checkoutSession.invoice) {
			return;
		}

		const reference = checkoutSession.payment_intent;

		/// If it is already processed, ignore.
		if (this.processed.has(reference)) {
			return;
		}

		const email = checkoutSession.customer_details.email;

		// Expand the line items
		const session = await this.stripe.checkout.sessions.retrieve(checkoutSession.id, {
			expand: ["line_items"],
		});

		let products = [];
		for (const item of session.line_items.data) {
			const product = await this.getProduct(item.price.product);
			products.push(product);
		}

		await this.options.callbackPayment(reference, email, products);
		this.processed.add(reference);
	}

	/// Process a given invoice, and retrieve its email, product list and recurrency.
	async processInvoice(invoice) {
		const reference = invoice.payment_intent;

		/// If it is already processed, ignore.
		if (this.processed.has(reference)) {
			return;
		}

		const email = invoice.customer_email;
		let maybeSubscription = null;
		if (invoice.subscription) {
			maybeSubscription = new Subscription(invoice.subscription, invoice.period_end);
		}

		let products = [];
		for (const item of invoice.lines.data) {
			const product = await this.getProduct(item.price.product);
			products.push(product);
		}

		await this.options.callbackPayment(reference, email, products, maybeSubscription);
		this.processed.add(reference);
	}

	/// Loop through all items of a specific list from the stripe API.
	async _forEachList(stripeListApi, callback) {
		let lastItem = null;
		let object = null;
		do {
			object = await stripeListApi(lastItem ? lastItem.id : undefined);
			for (lastItem of object.data) {
				await callback(lastItem);
			}
		} while (object.data.length > 0);
	}

	async _forEachSearch(stripeSearchApi, callback) {
		let page = null;
		do {
			const object = await stripeSearchApi(page ? page : undefined);
			for (const item of object.data) {
				await callback(item);
			}
			page = object.next_page;
		} while (page);
	}

	/// Trigger all non-processed payment, if any since a certain period in second until now.
	async trigger(periodS) {
		const now = Math.floor(Date.now() / 1000);
		const since = now - periodS;

		// Loop through all invoices first as they contain more data.
		await this._forEachSearch(
			async (page) => {
				return await this.stripe.invoices.search({
					query: "created>" + since + " AND status:'paid'",
					page: page,
				});
			},
			async (invoice) => await this.processInvoice(invoice),
		);

		// Loop through all checkout sessions.
		await this._forEachList(
			async (startingAfter) => {
				return await this.stripe.checkout.sessions.list({
					status: "complete",
					created: {
						gte: since,
					},
					starting_after: startingAfter,
				});
			},
			async (checkoutSession) => await this.processCheckoutSession(checkoutSession),
		);
	}
}
