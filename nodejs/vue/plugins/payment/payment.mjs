import Payment from "./payment.vue";

export default {
	install(Vue, payments) {
		Vue.component(
			"Payment",
			Vue.extend({
				mixins: [Payment],
				data() {
					return {
						payments: payments,
					};
				},
			})
		);
	},
};
