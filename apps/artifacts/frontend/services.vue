<template>
	<div v-loading="loading">
		{{ services }}
	</div>
</template>

<script>
	import Component from "bzd/vue/components/layout/component.vue";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";

	export default {
		mixins: [Component],
		directives: {
			loading: DirectiveLoading
		},
		data: function() {
			return {
				services: {}
			};
		},
		mounted() {
			this.fetchServices();
		},
		computed: {
		},
		methods: {
			async fetchServices() {
				await this.handleSubmit(async () => {
					this.services = await this.$api.request("get", "/services");
				});
			}
		}
	};
</script>
