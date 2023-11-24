<template>
	<div v-loading="loading" class="view">
		<div class="view-path">
			<template v-for="(segment, index) in pathList" :key="index">
				<span class="view-path-separator"> / </span>
				<code class="view-path-segment">
					<RouterLink :link="getLink(index)">{{ segment }}</RouterLink>
				</code>
			</template>
		</div>
		<component class="bzd-content" :is="viewComponent" :path-list="pathList"></component>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import { defineAsyncComponent } from "vue";

	import Plugins from "../plugins/frontend.mjs";

	const Exception = ExceptionFactory("view");

	export default {
		mixins: [Component],
		props: {
			path: { mandatory: true, type: String },
		},
		directives: {
			loading: DirectiveLoading,
		},
		data: function () {
			return {
				volumes: [],
				itemList: [],
			};
		},
		mounted() {
			this.fetchConfig();
		},
		emits: ["show"],
		watch: {
			path: {
				handler() {
					this.$emit("show", this.pathList);
				},
				immediate: true,
			},
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
					return defineAsyncComponent(() => this.plugin.view.apply(this));
				}
			},
		},
		methods: {
			async fetchConfig() {
				await this.handleSubmit(async () => {
					this.volumes = await this.$cache.get("list");
				});
			},
			getLink(index) {
				return (
					"/view/" +
					this.pathList
						.slice(0, index + 1)
						.map((segment) => encodeURIComponent(segment))
						.join("/")
				);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.view {
		margin: 0 !important;
		.view-path {
			font-size: 1.5em;
			line-height: 2em;
			padding: 0 10px;
			background-color: #eee;
		}
	}
</style>
