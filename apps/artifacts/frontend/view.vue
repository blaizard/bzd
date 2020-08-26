<template>
	<div v-loading="loading">
		<component :is="viewComponent" :path-list="pathList"></component>
	</div>
</template>

<script>
	import Component from "bzd/vue/components/layout/component.vue";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";
	import ExceptionFactory from "bzd/core/exception.mjs";

	import Plugins from "../plugins/frontend.mjs";

	const Exception = ExceptionFactory("view");

	export default {
		mixins: [Component],
		props: {
			path: { mandatory: true, type: String }
		},
		directives: {
			loading: DirectiveLoading
		},
		data: function() {
			return {
				volumes: [],
				itemList: []
			};
		},
		mounted() {
			this.fetchConfig();
		},
		computed: {
			pathList() {
				return this.path.split("/").map((c) => decodeURIComponent(c));
			},
			volume() {
				return this.pathList[0] || null;
			},
			plugin() {
				for (const item of this.volumes) {
					if (item.name == this.volume) {
						Exception.assert(item.plugin in Plugins, "Unsupported plugin '{}'", item.plugin);
						return Plugins[item.plugin];
					}
				}
				return null;
			},
			viewComponent() {
				if (this.plugin !== null) {
					return this.plugin.visualization;
				}
			}
		},
		methods: {
			async fetchConfig() {
				await this.handleSubmit(async () => {
					this.volumes = await this.$cache.get("list");
				});
			}
		}
	};
</script>
