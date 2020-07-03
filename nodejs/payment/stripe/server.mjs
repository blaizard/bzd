import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import Fetch from "../../core/fetch.mjs";
import APISchema from "./api.json";

const Exception = ExceptionFactory("payment", "stripe");
const Log = LogFactory("payment", "stripe");

export default class StripePayment {
	constructor(options) {
		this.options = Object.assign({
			/**
			 * The Stripe public key to be used by the client.
			 */
			publicKey: null,
			/**
			 * The Stripe private key to be used by the server.
			 */
			privateKey: null,
			/**
			 * The currency to be used.
			 */
			currency: "eur",
			/**
			 * Function to be called 
			 */
			intentCallback: (/*amount, metadata*/) => {}
		}, options);
	}

	installAPI(api) {
		Log.debug("Installing Stripe payment API.");
		api.addSchema(APISchema);

		api.handle("post", "/payment/stripe/intent", async (inputs/*, uid*/) => {

			this.options.intentCallback(inputs.amount, inputs.metadata);
	
			let amount = 0;
			switch (this.options.currency) {
			case "eur":
				amount = Math.floor(inputs.amount * 100);
				break;
			default:
				Exception.unreachable("Unsupported currency: {}", this.options.currency);
			}
			Exception.assert(amount, "Amount incorrect: {}", amount);

			const response = await Fetch.request("https://api.stripe.com/v1/payment_intents", {
				method: "post",
				authentication: {
					type: "basic",
					username: this.options.privateKey,
					password: ""
				},
				headers: {
					"Content-Type": "application/x-www-form-urlencoded"
				},
				expect: "json",
				data: "amount=" + amount + "&currency=" + this.options.currency + "&payment_method_types[]=card"
			});
			return {
				id: response.id,
				secret: response.client_secret
			};
		});

		api.handle("get", "/payment/stripe/metadata", async () => {
			return {
				pubkey: this.options.publicKey,
				currency: this.options.currency
			};
		});

		api.handle("post", "/payment/stripe/confirm", async (inputs) => {

			const response = await Fetch.request("https://api.stripe.com/v1/payment_intents/" + inputs.id, {
				method: "get",
				authentication: {
					type: "basic",
					username: this.options.privateKey,
					password: ""
				},
				expect: "json"
			});

			Exception.assert(response.amount == response.amount_received, "Payment has not been received");
		});
	}
}
