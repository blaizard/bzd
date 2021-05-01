<template>
	<div v-loading="loading">
		<h1>Services</h1>
		<table>
			<tr>
				<th>Volume</th>
				<th>Type</th>
				<th>Status</th>
			</tr>
			<template v-for="(service, volume) in services">
				<tr v-for="(type, index) in service" :key="index">
					<td>{{ volume }}</td>
					<td>{{ type }}</td>
					<td>Running</td>
				</tr>
			</template>
		</table>
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
		computed: {},
		methods: {
			async fetchServices() {
				await this.handleSubmit(async () => {
					this.services = await this.$api.request("get", "/services");
				});
			}
		}
	};
</script>
