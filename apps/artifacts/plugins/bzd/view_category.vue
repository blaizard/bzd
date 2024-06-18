<template>
	<div class="data">
		<div class="entry" v-for="[path, value] of pathValues">
			<span class="path">{{ path }} ({{ value["timestamp"] }})</span>
			<span>{{ formatValue(value["value"]) }}</span>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import HttpClient from "#bzd/nodejs/core/http/client.mjs";

	export default {
		mixins: [Base, Component],
		data: function () {
			return {
				metadata: {},
			};
		},
		computed: {
			pathValues() {
				return Object.entries(this.metadata).sort((a, b) => a[0].localeCompare(b[0]));
			},
		},
		mounted() {
			this.fetchMetadata();
		},
		methods: {
			formatValue(value) {
				return JSON.stringify(value, null, 4);
			},
			async fetchMetadata() {
				await this.handleSubmit(async () => {
					const endpoint = "/x/" + this.pathList.map(encodeURIComponent).join("/");
					this.metadata = await HttpClient.get(endpoint, {
						expect: "json",
					});
				});
			},
		},
	};
</script>

<style lang="scss" scoped>
	.data {
		display: flex;
		flex-direction: column;

		.entry {
			display: flex;
			flex-direction: row;

			.path {
				font-weight: bold;
			}
		}
	}
</style>
