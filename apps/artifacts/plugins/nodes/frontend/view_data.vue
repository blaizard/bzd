<template>
	<div class="data" v-if="metadata">
		<div>Updated: {{ durationString }} ago</div>
		<Keys class="keys" :value="tree" :path-list="pathList" v-slot="serializeSlotProps" @select="onSelect">
			<Value v-if="isPathListValue(serializeSlotProps.pathList)" :value="serializeSlotProps.value" :view="0"></Value>
			<Value v-else :value="serializeSlotProps.value" :view="1" @select="onSelect(serializeSlotProps.pathList)"></Value>
		</Keys>
		<div>
			<span>Accessors:</span>
			<ul>
				<li v-for="accessor in endpointsAccessors">
					<a :href="accessor">{{ accessor }}</a>
				</li>
			</ul>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Value from "#bzd/apps/artifacts/plugins/nodes/frontend/value.vue";
	import Keys from "#bzd/apps/artifacts/plugins/nodes/frontend/keys.vue";
	import { timeMsToString } from "#bzd/nodejs/utils/to_string.mjs";
	import Utils from "#bzd/apps/artifacts/common/utils.mjs";

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
				timestampNewest: 0,
				timeout: null,
				isDestroyed: false,
			};
		},
		computed: {
			durationString() {
				return timeMsToString(Math.max(this.timestampServer - this.timestampNewest, 0));
			},
			endpoint() {
				return "/x" + Utils.keyToPath(this.pathList);
			},
			endpointExport() {
				const [volume, ...path] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@export" + Utils.keyToPath(path);
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
			endpointsAccessors() {
				if (this.isValue) {
					if (this.isChildren) {
						return [
							this.endpoint + "?children=99",
							this.endpoint + "?metadata=1",
							this.endpoint + "?count=5",
							this.endpointExport + "?children=99&format=csv",
						];
					}
					return [this.endpoint + "?metadata=1", this.endpoint + "?count=5", this.endpointExport + "?format=csv"];
				}
				return [
					this.endpoint + "?children=99",
					this.endpoint + "?children=99&metadata=1",
					this.endpoint + "?children=99&count=5",
					this.endpointExport + "?children=99&format=csv",
				];
			},
			timestampServer() {
				return this.metadata.timestamp || Date.now();
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
			isPathListValue(pathList) {
				return (
					this.pathList.length === pathList.length && this.pathList.every((entry, index) => entry == pathList[index])
				);
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

				// Keep only the last X values depending if root value or not.
				const keepLastN = key.length === 0 ? 100 : 10;
				object["_"] = object["_"].slice(-keepLastN);

				// Update the last timestamp.
				const last = object["_"].slice(-1)[0];
				this.timestampNewest = Math.max(this.timestampNewest, last[0]);
			},
			async fetchMetadata() {
				await this.handleSubmit(async () => {
					let query = { metadata: 1, children: 99, count: 1 };
					if (this.timestampNewest) {
						query.after = this.timestampNewest;
						query.count = 5;
					}
					this.metadata = await this.requestBackend(this.endpoint, {
						method: "get",
						query: query,
						expect: "json",
					});
					for (const [key, value] of this.metadata.data) {
						this.updateTree(key, value);
					}
				});

				if (this.isValue) {
					await this.handleSubmit(async () => {
						const value = await this.requestBackend(this.endpoint, {
							method: "get",
							query: { metadata: 1, count: 10, before: this.valueOldestTimestamp },
							expect: "json",
						});
						this.updateTree([], value.data);
					});
				}

				if (!this.isDestroyed) {
					this.timeout = setTimeout(this.fetchMetadata, 1000);
				}
			},
			onSelect(pathList) {
				const url = "/view/" + Utils.keyToPath(pathList);
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
		}
	}
</style>
