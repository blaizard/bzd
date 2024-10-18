<template>
	<div class="data" v-if="metadata">
		<div>Updated: {{ durationString }} ago</div>
		<Keys class="keys" :value="tree" :path-list="pathList" v-slot="serializeSlotProps" @select="onSelect($event)">
			<Value
				class="value"
				:value="serializeSlotProps.value"
				:view="isPathListValue(serializeSlotProps.pathList) ? 0 : 1"
				v-slot="valueSlotProps"
			>
				<code class="json">{{ JSON.stringify(valueSlotProps.value) }}</code>
			</Value>
		</Keys>
		<div>
			<span>Accessors:</span>
			<ul>
				<li>
					<a :href="endpoint + '?children=1'">{{ endpoint }}?children=1</a>
				</li>
				<li>
					<a :href="endpoint + '?metadata=1'">{{ endpoint }}?metadata=1</a>
				</li>
				<li>
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
	import Keys from "#bzd/apps/artifacts/plugins/nodes/keys.vue";
	import { timeMsToString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		mixins: [Base, Component],
		components: {
			Value,
			Keys,
		},
		data: function () {
			return {
				metadata: {},
				tree: {},
				timestamp: 0,
				timeout: null,
			};
		},
		computed: {
			durationString() {
				return timeMsToString(this.metadata.timestampServer - this.timestamp);
			},
			endpoint() {
				return "/x/" + this.pathList.map(encodeURIComponent).join("/");
			},
			isValue() {
				return "_" in this.tree;
			},
			value() {
				return this.tree["_"] || [];
			},
			valueOldestTimestamp() {
				return this.value[0][0];
			},
		},
		mounted() {
			this.fetchMetadata();
		},
		beforeUnmount() {
			clearTimeout(this.timeout);
		},
		methods: {
			isPathListValue(pathList) {
				const mustMatch = [...this.pathList, "_"];
				return mustMatch.length === pathList.length && mustMatch.every((entry, index) => entry == pathList[index]);
			},
			updateTree(key, values) {
				// Update the tree.
				const object = key.reduce((r, segment) => {
					r[segment] ??= {};
					return r[segment];
				}, this.tree);
				object["_"] ??= [];
				object["_"].push(...values);
				object["_"].sort((a, b) => {
					return a[0] > b[0] ? 1 : -1;
				});

				// Update the timestamp.
				const last = object["_"].slice(-1)[0];
				this.timestamp = Math.max(this.timestamp, last[0]);
			},
			async fetchMetadata() {
				await this.handleSubmit(async () => {
					let query = { metadata: 1, children: 1, count: 1 };
					if (this.timestamp) {
						query.after = this.timestamp;
						query.count = 5;
					}
					this.metadata = await HttpClient.get(this.endpoint, {
						query: query,
						expect: "json",
					});
					for (const [key, value] of this.metadata.data) {
						this.updateTree(key, value);
					}
				});

				if (this.isValue) {
					await this.handleSubmit(async () => {
						const value = await HttpClient.get(this.endpoint, {
							query: { metadata: 1, count: 10, before: this.valueOldestTimestamp },
							expect: "json",
						});
						this.updateTree([], value.data);
					});
				}

				this.timeout = setTimeout(this.fetchMetadata, 1000);
			},
			onSelect(pathList) {
				const url = "/view/" + pathList.map(encodeURIComponent).join("/");
				this.$router.dispatch(url);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.data {
		display: flex;
		flex-direction: column;

		.keys {
			border: 1px solid #eee;
			padding: 10px;

			.value {
				&:hover {
					text-shadow: 1px 0 #000;
				}

				.json {
					background-color: transparent;
				}
			}
		}
	}
</style>
