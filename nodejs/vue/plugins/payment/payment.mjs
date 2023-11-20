import Payment from "./payment.vue";

export default {
	install(Vue, payments) {
		Vue.component(
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
