<template>
	<div class="bzd-payments">
		<component
			v-for="(payment, index) in payments"
			:key="index"
			:is="componentList[index]"
			:amount="amount"
			:metadata="metadata"
		></component>
	</div>
</template>

<script>
import Vue from "vue";

export default {
	props: {
		amount: { type: Number, mandatory: true },
		metadata: { type: Object, mandatory: false, default: () => ({}) },
	},
	data() {
		return {
			componentList: [],
		};
	},
	mounted() {
		for (const index in this.payments) {
			this.componentList.push(this.getComponent(this.payments[index]));
		}
	},
	methods: {
		getComponent(payment) {
			return () =>
				payment.getComponent(
					Vue,
					() => {
						this.$notification.success({ "bzd-payment": "Payment received" });
					},
					(message = null) => {
						this.$notification.error({ "bzd-payment": message });
					}
				);
		},
	},
};
</script>

<style lang="scss">
	.bzd-payments {
	}
</style>
