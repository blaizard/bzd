<template>
	<div class="data">
		<div>
			Updated: <span>{{ duration }}</span> ago
		</div>
		<pre><code>{{ formatValue(metadata.data) }}</code></pre>
		<!--<div class="entry" v-for="[path, value] of pathValues">
			<span class="path">{{ path }} ({{ value["timestamp"] }})</span>
			<span>{{ formatValue(value["value"]) }}</span>
		</div>//-->
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import HttpClient from "#bzd/nodejs/core/http/client.mjs";
	import { timeMsToString } from "#bzd/nodejs/utils/to_string.mjs";
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
			duration() {
				return timeMsToString(this.metadata.timestampServer - this.metadata.timestamp);
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
