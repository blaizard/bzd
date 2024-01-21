import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Stripe from "stripe";
import ServiceProvider from "#bzd/apps/accounts/backend/services/provider.mjs";

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
				/// \return true if it was processed, false otherwise.
				callbackPayment: null,
				/// Period to perform a sync with the server.
				syncPeriodS: 6 * 3600,
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

		/// This webhook must be setup in https://dashboard.stripe.com/webhooks
		///
		/// The following events must be setup:
		/// - checkout.session.completed
		/// - invoice.payment_succeeded
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

	async installService(service) {
		Log.info("Installing 'Stripe' service");

		const provider = new ServiceProvider("payment", "stripe");
		provider.addTimeTriggeredProcess(
			"sync",
			async () => {
				return await this.sync(this.options.syncPeriodS * 2);
			},
			{
				periodS: this.options.syncPeriodS,
				// It must start at the very begining to minimize the delay when the server restarts.
				delayS: 0,
			},
		);
		service.register(provider);
	}

	async getProduct(productId) {
		if (productId in this.products) {
			return this.products[productId];
		}
		const product = await this.stripe.products.retrieve(productId);
		this.products[productId] = product;
		return product;
	}

	/// Process a given checkout session.
	///
	/// Return true if it was processed, false otherwise.
	async processCheckoutSession(checkoutSession) {
		// Ignore if there is an invoice, it will be handled by the invoice instead.
		if (checkoutSession.invoice) {
			return false;
		}

		const reference = checkoutSession.payment_intent;

		/// If it is already processed, ignore.
		if (this.processed.has(reference)) {
			return false;
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

		const isProcessed = await this.options.callbackPayment(reference, email, products);
		this.processed.add(reference);
		return isProcessed;
	}

	/// Process a given invoice, and retrieve its email, product list and recurrency.
	///
	/// Return true if it was processed, false otherwise.
	async processInvoice(invoice) {
		const reference = invoice.payment_intent;

		/// If it is already processed, ignore.
		if (this.processed.has(reference)) {
			return false;
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

		const isProcessed = await this.options.callbackPayment(reference, email, products, maybeSubscription);
		this.processed.add(reference);
		return isProcessed;
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
	async sync(periodS) {
		const now = Math.floor(Date.now() / 1000);
		const since = now - periodS;

		// Loop through all invoices first as they contain more data.
		let invoices = 0;
		await this._forEachSearch(
			async (page) => {
				return await this.stripe.invoices.search({
					query: "created>" + since + " AND status:'paid'",
					page: page,
				});
			},
			async (invoice) => {
				if (await this.processInvoice(invoice)) {
					++invoices;
				}
			},
		);

		// Loop through all checkout sessions.
		let checkouts = 0;
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
			async (checkoutSession) => {
				if (await this.processCheckoutSession(checkoutSession)) {
					++checkouts;
				}
			},
		);

		return { invoices, checkouts };
	}
}
