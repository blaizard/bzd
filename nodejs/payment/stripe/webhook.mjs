import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Stripe from "stripe";
import ServiceProvider from "#bzd/nodejs/core/services/provider.mjs";
import { PaymentInterface, PaymentRecurrency } from "#bzd/nodejs/payment/payment.mjs";
import Result from "#bzd/nodejs/utils/result.mjs";

const Exception = ExceptionFactory("payment", "stripe");
const Log = LogFactory("payment", "stripe");

export default class StripePaymentWebhook extends PaymentInterface {
	constructor(callbackPayment, callbackCancelRecurrency, options) {
		super(callbackPayment, callbackCancelRecurrency);
		this.options = Object.assign(
			{
				/// You API secret Key.
				secretKey: null,
				/// The endpoint secret.
				secretEndpoint: null,
				/// Period to perform a sync with the server.
				syncPeriodS: 6 * 3600,
			},
			options,
		);

		this._init();
		// Map containing the matching between product identifier and the actual product.
		this.products = {};
		// Map containing the matching between customer identifier and their email.
		this.customers = {};
		// Contain all already processed uids .
		this.processedPayments = new Set();
		this.processedDeletedSubscriptions = new Set();
	}

	/// Create the stripe object.
	_init() {
		Exception.assert(this.options.test || this.options.secretKey, "'secretKey' is missing.");
		Exception.assert(this.options.test || this.options.secretEndpoint, "'secretEndpoint' is missing.");
		this.stripe = Stripe(this.options.secretKey);
	}

	installRest(rest) {
		Log.debug("Installing Stripe webhook REST.");

		const self = this;

		/// This webhook must be setup in https://dashboard.stripe.com/webhooks
		///
		/// The following events must be setup:
		/// - checkout.session.completed
		/// - invoice.payment_succeeded
		rest.handle("post", "/stripe", async function (inputs) {
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

				case "customer.subscription.deleted":
					await self.processSubscriptionDeleted(event.data.object);
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
				// It must start at the very beginning to minimize the delay when the server restarts.
				delayS: 0,
			},
		);
		service.register(provider);
	}

	/// A product is mapped with the metadata field in stripe.
	async getProduct(productId) {
		if (productId in this.products) {
			return this.products[productId];
		}
		const product = await this.stripe.products.retrieve(productId);
		this.products[productId] = Object.assign(
			{
				uid: productId,
			},
			product.metadata,
		);
		return this.products[productId];
	}

	/// Return the email associated with a customer from its ID.
	async getCustomerEmail(customerId) {
		if (customerId in this.customers) {
			return this.customers[customerId];
		}
		const customer = await this.stripe.customers.retrieve(customer);
		this.customers[customerId] = customer.email;
		return this.customers[customerId];
	}

	/// Process a deleted subscription event.
	async processSubscriptionDeleted(subscription) {
		const uid = subscription.id;

		if (this.processedDeletedSubscriptions.has(uid)) {
			return;
		}

		const email = await this.getCustomerEmail(subscription.customer);

		try {
			const isProcessed = await this.callbackCancelRecurrency(uid, email);
			this.processedDeletedSubscriptions.add(uid);
			return isProcessed;
		} catch (e) {
			Log.error("Cancel Recurrency: {:j}", subscription);
			throw e;
		}
	}

	/// Process a given checkout session.
	///
	/// Return true if it was processed, false otherwise.
	async processCheckoutSession(checkoutSession) {
		// Ignore if there is an invoice, it will be handled by the invoice instead.
		if (checkoutSession.invoice) {
			return false;
		}

		const uid = checkoutSession.payment_intent;

		/// If it is already processed, ignore.
		if (this.processedPayments.has(uid)) {
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

		try {
			const isProcessed = await this.callbackPayment(uid, email, products);
			this.processedPayments.add(uid);
			return isProcessed;
		} catch (e) {
			Log.error("Checkout Session: {:j}", checkoutSession);
			throw e;
		}
	}

	/// Process a given invoice, and retrieve its email, product list and recurrency.
	///
	/// Return true if it was processed, false otherwise.
	async processInvoice(invoice) {
		const uid = invoice.payment_intent;

		/// If it is already processed, ignore.
		if (this.processedPayments.has(uid)) {
			return false;
		}

		const email = invoice.customer_email;

		let products = [];
		let perdiodEnd = 0;
		for (const item of invoice.lines.data) {
			const product = await this.getProduct(item.price.product);
			products.push(product);
			perdiodEnd = Math.max(perdiodEnd, item.period.end);
		}

		let maybeRecurrency = null;
		if (invoice.subscription && perdiodEnd) {
			maybeRecurrency = new PaymentRecurrency(invoice.subscription, perdiodEnd * 1000);
		}

		try {
			const isProcessed = await this.callbackPayment(uid, email, products, maybeRecurrency);
			this.processedPayments.add(uid);
			return isProcessed;
		} catch (e) {
			Log.error("Invoice: {:j}", invoice);
			throw e;
		}
	}

	/// Loop through all items of a specific list from the stripe API.
	async _forEachList(stripeListApi, callback) {
		let lastItem = null;
		let object = null;
		let errors = [];
		do {
			object = await stripeListApi(lastItem ? lastItem.id : undefined);
			for (lastItem of object.data) {
				try {
					await callback(lastItem);
				} catch (e) {
					errors.push(String(e));
				}
			}
		} while (object.data.length > 0);

		if (errors.length) {
			return Result.makeError(errors);
		}
		return new Result();
	}

	async _forEachSearch(stripeSearchApi, callback) {
		let page = null;
		let errors = [];
		do {
			const object = await stripeSearchApi(page ? page : undefined);
			for (const item of object.data) {
				try {
					await callback(item);
				} catch (e) {
					errors.push(String(e));
				}
			}
			page = object.next_page;
		} while (page);

		if (errors.length) {
			return Result.makeError(errors);
		}
		return new Result();
	}

	/// Trigger all non-processed payment, if any since a certain period in second until now.
	async sync(periodS) {
		const now = Math.floor(Date.now() / 1000);
		const since = now - periodS;

		// Loop through all invoices first as they contain more data.
		let invoices = [0, 0];
		const resultInvoices = await this._forEachSearch(
			async (page) => {
				return await this.stripe.invoices.search({
					query: "created>" + since + " AND status:'paid'",
					page: page,
				});
			},
			async (invoice) => {
				++invoices[1];
				if (await this.processInvoice(invoice)) {
					++invoices[0];
				}
			},
		);

		// Loop through all checkout sessions.
		let checkouts = [0, 0];
		const resultCheckout = await this._forEachList(
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
				++checkouts[1];
				if (await this.processCheckoutSession(checkoutSession)) {
					++checkouts[0];
				}
			},
		);

		// Loop through all cancelled subscriptions.
		let subscriptions = [0, 0];
		const resultSubscriptions = await this._forEachSearch(
			async (page) => {
				return await this.stripe.subscriptions.search({
					query: "created>" + since + " AND status:'canceled'",
					page: page,
				});
			},
			async (subscription) => {
				++subscriptions[1];
				if (await this.processSubscriptionDeleted(subscription)) {
					++subscriptions[0];
				}
			},
		);

		let errors = [];
		if (resultInvoices.hasError()) {
			errors = [...errors, resultInvoices.error()];
		}
		if (resultCheckout.hasError()) {
			errors = [...errors, resultCheckout.error()];
		}
		if (resultSubscriptions.hasError()) {
			errors = [...errors, resultSubscriptions.error()];
		}
		Exception.assert(errors.length == 0, "{} error(s): {}", errors.length, errors.join("\n"));

		return { invoices, checkouts, subscriptions };
	}

	/// Cancel an existing payment recurrency.
	async _cancelRecurrency(uid, email) {
		await this.stripe.subscriptions.cancel(uid);
	}
}
