<template>
	<div class="bzd-dashboard-instances">
		<Instance v-for="(description, uid) in instances" :key="uid" :uid="uid" :description="description"> </Instance>
	</div>
</template>

<script>
	import Instance from "./instance.vue";

	export default {
		components: {
			Instance,
		},
		props: {
			another: { type: String, mandatory: false, default: null },
		},
		data: function () {
			return {
				instances: {},
			};
		},
		mounted() {
			this.getInstances();
		},
		methods: {
			async getInstances() {
				this.instances = await this.$rest.request("get", "/instances");
			},
		},
	};
</script>

<style lang="scss">
	.bzd-dashboard-instances {
		display: flex;
		flex-flow: row wrap;
	}
</style>
