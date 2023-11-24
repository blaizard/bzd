import Payment from "./payment.vue";

export default {
	install(app, payments) {
		app.component(
			"Payment",
			Payment.extend({
				data() {
					return {
						payments: payments,
					};
				},
			}),
		);
	},
};
