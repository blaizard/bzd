import LogFactory from "../../core/log.mjs";

import APISchema from "./api.json" assert { type: "json" };
import VueComponent from "./client.vue";

const Log = LogFactory("payment", "stripe");

export default class StripePayment {
	constructor(options) {
		this.options = Object.assign(
			{
				/**
				 * Callback to create a payment intent.
				 */
				makePaymentIntent: null,
				/**
				 * Callback to retreive the public key.
				 */
				getMetadata: null,
				/**
				 * Callback to confirm the payment.
				 */
				confirmPayment: null,
			},
			options,
		);

		this.card = null;
	}

	async installAPI(api) {
		Log.debug("Installing Stripe payment API.");

		if (!this.options.makePaymentIntent) {
			this.options.makePaymentIntent = async (amount, metadata) => {
				return await api.request("post", "/payment/stripe/intent", {
					amount: amount,
					metadata: metadata,
				});
			};
		}
		if (!this.options.getMetadata) {
			this.options.getMetadata = async () => {
				return await api.request("get", "/payment/stripe/metadata");
			};
		}
		if (!this.options.confirmPayment) {
			this.options.confirmPayment = async (intentId, metadata) => {
				return await api.request("post", "/payment/stripe/confirm", {
					id: intentId,
					metadata: metadata,
				});
			};
		}
	}

	async getComponent(Vue, handleSuccess = () => {}, handleError = () => {}) {
		const getMetadata = await this.options.getMetadata;
		const makePaymentIntent = this.options.makePaymentIntent;
		const confirmPayment = this.options.confirmPayment;

		return VueComponent.extend({
			methods: {
				async makePaymentIntent(amount, metadata) {
					return await makePaymentIntent(amount, metadata);
				},
				async getMetadata() {
					return await getMetadata();
				},
				async confirmPayment(intentId, metadata) {
					await confirmPayment(intentId, metadata);
					await handleSuccess();
				},
				handleError(message) {
					handleError(message);
				},
			},
		});
	}
}
