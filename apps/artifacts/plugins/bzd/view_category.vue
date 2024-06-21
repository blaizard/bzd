<template>
	<div class="data" v-if="metadata">
		<div>Updated: {{ durationString }} ago</div>
		<pre><code>{{ formatValue(processedData) }}</code></pre>
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
				metadata: null,
			};
		},
		computed: {
			timestamp() {
				let timestamp = 0;
				this.iterateMetadata((t, v) => {
					timestamp = Math.max(timestamp, t);
				});
				return timestamp;
			},
			processedData() {
				return this.iterateMetadata((t, v) => {
					return v;
				});
			},
			durationString() {
				return timeMsToString(this.metadata.timestampServer - this.timestamp);
			},
		},
		mounted() {
			this.fetchMetadata();
		},
		methods: {
			/// Iterate through the metadata and return a copy of it.
			iterateMetadata(callback) {
				const isValue = (data) => Array.isArray(data) && data.length == 2;
				const recursive = (object, callback) => {
					let updatedObject = {};
					Object.entries(object).forEach(([key, data]) => {
						if (isValue(data)) {
							updatedObject[key] = callback(data[0], data[1]);
						} else if (data && typeof data === "object") {
							updatedObject[key] = recursive(data, callback);
						}
					});
					return updatedObject;
				};
				const object = this.metadata.data;
				if (isValue(object)) {
					return callback(object[0], object[1]);
				}
				return recursive(object, callback);
			},
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
				setTimeout(this.fetchMetadata, 1000);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.data {
		display: flex;
		flex-direction: column;
	}
</style>
