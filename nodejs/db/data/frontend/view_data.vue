<template>
	<div class="data" v-if="metadata">
		<!-- Search input -->
		<div class="search">
			<Input :description="{ placeholder: 'Filter...' }" @directInput="onFilter" />
		</div>
		<!-- Tree view: shown when filter is empty -->
		<div v-if="isEmpty">
			<div>Updated: {{ durationString }} ago</div>
			<Keys class="keys" :value="tree" :path-list="pathList" v-slot="serializeSlotProps" @select="propagateOnSelect">
				<Value
					v-if="isPathListValue(serializeSlotProps.pathList)"
					:value="serializeSlotProps.value"
					:view="0"
					:timestamp="timestampServer"
				></Value>
				<Value
					v-else
					:value="serializeSlotProps.value"
					:view="1"
					:timestamp="timestampServer"
					@select="propagateOnSelect(serializeSlotProps.pathList)"
				></Value>
			</Keys>
		</div>
		<!-- Flat list: shown when filter is non-empty -->
		<div v-else>
			<div>Updated: {{ durationString }} ago</div>
			<div class="entries">
				<div v-for="entry in filteredEntries" :key="entry.key" class="entry">
					<span class="key">{{ entry.key }}</span>
					<Value :value="entry.value" :view="1" :timestamp="timestampServer"></Value>
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Value from "#bzd/nodejs/db/data/frontend/value.vue";
	import Keys from "#bzd/nodejs/db/data/frontend/keys.vue";
	import Input from "#bzd/nodejs/vue/components/form/element/input.vue";
	import { timeToString } from "#bzd/nodejs/utils/to_string.js";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";

	export default {
		mixins: [Component],
		components: {
			Value,
			Keys,
			Input,
			Form,
		},
		data: function () {
			return {
				metadata: {},
				tree: {},
				flat: {},
				timestampNewest: 0,
				timeout: null,
				isDestroyed: false,
				filter: "",
			};
		},
		emits: ["select"],
		props: {
			apiGet: { mandatory: true, type: Function },
			pathList: { mandatory: true, type: Array },
		},
		computed: {
			durationString() {
				return timeToString(Math.max(this.timestampServer - this.timestampNewest, 0) / 1000);
			},
			isValue() {
				return "_" in this.tree;
			},
			isChildren() {
				for (const key of Object.keys(this.tree)) {
					if (key != "_") {
						return true;
					}
				}
				return false;
			},
			value() {
				return this.tree["_"] || [];
			},
			valueOldestTimestamp() {
				return this.value[0][0];
			},
			timestampServer() {
				return this.metadata.timestamp || Date.now();
			},
			isEmpty() {
				return this.filter.trim() === "";
			},
			filteredEntries() {
				const needle = this.filter.toLowerCase();
				const out = [];
				for (const [key, values] of Object.entries(this.flat)) {
					if (needle && !key.toLowerCase().includes(needle)) {
						continue;
					}
					if (!values.length) {
						continue;
					}
					out.push({ key: key, value: values });
				}
				out.sort((a, b) => a.key.localeCompare(b.key));
				return out;
			},
		},
		mounted() {
			this.fetchMetadata();
		},
		beforeUnmount() {
			this.isDestroyed = true;
			clearTimeout(this.timeout);
		},
		methods: {
			onFilter(text) {
				this.filter = text;
			},
			isPathListValue(pathList) {
				return (
					this.pathList.length === pathList.length && this.pathList.every((entry, index) => entry == pathList[index])
				);
			},
			updateTree(key, values) {
				const object = key.reduce((r, segment) => {
					r[segment] ??= {};
					return r[segment];
				}, this.tree);
				object["_"] ??= [];
				object["_"].push(...values);
				object["_"].sort((a, b) => {
					return b[0] - a[0];
				});
				const keepLastN = key.length === 0 ? 100 : 10;
				object["_"] = object["_"].slice(0, keepLastN);
				const newest = object["_"][0];
				this.timestampNewest = Math.max(this.timestampNewest, newest[0]);

				// Update the flat view as well to search by keys.
				this.flat[key.join(".")] = object["_"];
			},
			async fetchMetadata() {
				await this.handleSubmit(async () => {
					let query = { metadata: 1, children: 99, count: 1 };
					if (this.timestampNewest) {
						query.after = this.timestampNewest;
						query.count = 5;
					}
					this.metadata = await this.apiGet(query);
					for (const [key, value] of this.metadata.data) {
						this.updateTree(key, value);
					}
				});
				if (this.isValue) {
					await this.handleSubmit(async () => {
						const value = await this.apiGet({ metadata: 1, count: 10, before: this.valueOldestTimestamp });
						this.updateTree([], value.data);
					});
				}
				if (!this.isDestroyed) {
					this.timeout = setTimeout(this.fetchMetadata, 1000);
				}
			},
			propagateOnSelect(pathList) {
				this.$emit("select", pathList);
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
		}
		.search {
			margin-bottom: 16px;
		}
		.entries {
			display: flex;
			flex-direction: column;
			border: 1px solid #eee;
			padding: 10px;
		}
		.entry {
			display: flex;
			align-items: baseline;
			padding: 2px 0;
			.key {
				margin-right: 8px;
			}
		}
	}
</style>
