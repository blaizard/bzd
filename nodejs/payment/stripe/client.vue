<template>
	<div class="bzd-payment-stripe" v-loading="loading">
		<div ref="card" class="bzd-payment-stripe-card"></div>
		<Button action="approve" :content="payContent" @click="handleSubmit"></Button>
		<div class="bzd-payment-stripe-error">{{ error }}</div>
	</div>
</template>

<script>
	import HttpClient from "../../core/http/client.mjs";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import Button from "#bzd/nodejs/vue/components/form/element/button.vue";
	import Colors from "#bzd/nodejs/styles/default/css/colors.module.scss";

	export default {
		props: {
			amount: { type: Number, mandatory: true },
			metadata: { type: Object, mandatory: false, default: () => ({}) },
		},
		directives: {
			loading: DirectiveLoading,
		},
		components: {
			Button,
		},
		async mounted() {
			this.stripeMetadata = await this.getMetadata();
			this.stripe = (await this._getStripeJs())(this.stripeMetadata.pubkey);
			const elements = this.stripe.elements();
			const style = {
				base: {
					color: Colors.black,
					fontFamily: '"Helvetica Neue", Helvetica, sans-serif',
					fontSmoothing: "antialiased",
					fontSize: "16px",
					"::placeholder": {
						color: Colors.gray,
					},
				},
				invalid: {
					color: Colors.red,
					iconColor: Colors.red,
				},
			};
			this.card = elements.create("card", { style: style });
			this.card.mount(this.$refs.card);
			this.card.on("change", ({ error }) => {
				error ? this.setError(error.message) : this.clearError();
			});
		},
		data() {
			return {
				stripeMetadata: {},
				stripe: null,
				card: null,
				error: null,
				loading: false,
			};
		},
		computed: {
			payContent() {
				let message = "Pay";
				if ("currency" in this.stripeMetadata) {
					const currencyMap = {
						eur: "€",
						usd: "$",
						ron: "L",
					};
					const currency = currencyMap[this.stripeMetadata.currency] || null;
					if (currency) {
						message += " " + this.amount + currency;
					}
				}
				return message;
			},
		},
		methods: {
			async _getStripeJs() {
				if (!("Stripe" in window)) {
					const stripeJs = await HttpClient.request("https://js.stripe.com/v3/", {
						method: "get",
					});
					eval(stripeJs);
				}
				return window.Stripe;
			},
			setError(message) {
				this.error = message;
				this.handleError(message);
			},
			clearError() {
				this.setError(null);
			},
			async handleSubmit() {
				this.clearError();
				this.loading = true;
				try {
					const intent = await this.makePaymentIntent(this.amount, this.metadata);
					const result = await this.stripe.confirmCardPayment(intent.secret, {
						// eslint-disable-next-line
						payment_method: {
							card: this.card,
							// eslint-disable-next-line
							billing_details: {
								name: "Jenny Rosen",
							},
						},
					});

					if (result.error) {
						this.setError(result.error.message);
					} else {
						if (result.paymentIntent.status === "succeeded") {
							await this.confirmPayment(intent.id, this.metadata);
						}
					}
				} finally {
					this.loading = false;
				}
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/base.scss" as base;
	@use "#bzd/config.scss" as config;

	.bzd-payments {
		.bzd-payment-stripe-card {
			max-width: 400px;
			padding-top: 4px;
			display: inline-block;
			@extend %field-reset;
			@extend %placeholder;
			@extend %border;
		}
		.bzd-payment-stripe-error {
			font-size: 0.8em;
			color: config.$bzdGraphColorRed;
		}
	}
</style>
