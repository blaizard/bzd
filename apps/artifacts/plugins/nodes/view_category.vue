<template>
	<div class="data" v-if="metadata">
		<div>Updated: {{ durationString }} ago</div>
		<Serialize class="value" :value="metadataTree" :path-list="pathList" v-slot="serializeSlotProps">
			<Value :value="serializeSlotProps.value" :path-list="serializeSlotProps.pathList" v-slot="valueSlotProps">
				<Serialize :value="valueSlotProps.value"></Serialize>
			</Value>
		</Serialize>
		<div>
			<span v-if="isValue(value)">Accessors for {{ pathList.join("/") }}:</span>
			<span v-else>Accessors:</span>
			<ul>
				<li>
					<a :href="endpoint">{{ endpoint }}</a>
				</li>
				<li>
					<a :href="endpoint + '?metadata=1'">{{ endpoint }}?metadata=1</a>
				</li>
				<li v-if="isValue(value)">
					<a :href="endpoint + '?count=5'">{{ endpoint }}?count=5</a>
				</li>
			</ul>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import HttpClient from "#bzd/nodejs/core/http/client.mjs";
	import Value from "#bzd/apps/artifacts/plugins/nodes/value.vue";
	import Serialize from "#bzd/apps/artifacts/plugins/nodes/serialize.vue";
	import { timeMsToString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		mixins: [Base, Component],
		components: {
			Value,
			Serialize,
		},
		data: function () {
			return {
				metadata: null,
				timeout: null,
			};
		},
		computed: {
			metadataTree() {
				if (!this.metadata) {
					return {};
				}
				let tree = {};
				for (const [key, value] of this.metadata.data) {
					const object = key.reduce((r, segment) => {
						r[segment] ??= {};
						return r[segment];
					}, tree);
					object["_"] = value;
				}
				return tree;
			},
			timestamp() {
				let timestamp = 0;
				this.iterateMetadata((t, v) => {
					timestamp = Math.max(timestamp, t);
				});
				return timestamp;
			},
			durationString() {
				return timeMsToString(this.metadata.timestampServer - this.timestamp);
			},
			endpoint() {
				return "/x/" + this.pathList.map(encodeURIComponent).join("/");
			},
			value() {
				return this.metadata.data || {};
			},
		},
		mounted() {
			this.fetchMetadata();
		},
		beforeUnmount() {
			clearTimeout(this.timeout);
		},
		methods: {
			isValue(value) {
				return Array.isArray(value);
			},
			/// Iterate through the metadata and return a copy of it.
			iterateMetadata(callback) {
				if (this.metadata) {
					for (const [key, data] of this.metadata.data) {
						callback(data[0], data[1]);
					}
				}
			},
			async fetchMetadata() {
				await this.handleSubmit(async () => {
					this.metadata = await HttpClient.get(this.endpoint, {
						query: { metadata: 1, children: 1 },
						expect: "json",
					});
				});
				this.timeout = setTimeout(this.fetchMetadata, 1000);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.data {
		display: flex;
		flex-direction: column;

		.value {
			border: 1px solid #eee;
			padding: 10px;
		}
	}
</style>
