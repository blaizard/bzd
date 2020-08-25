<template>
	<div v-loading="loading">
		{{ pathList }}
		<div>{{ item }}</div>
	</div>
</template>

<script>
	import Component from "bzd/vue/components/layout/component.vue";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";

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
				itemList: []
			};
		},
		mounted() {
			if (this.volume !== null) {
				this.fetchConfig();
			}
		},
		computed: {
			pathList() {
				return this.path.split("/").map((c) => decodeURIComponent(c));
			},
			name() {
				return this.pathList.slice(-1);
			},
			item() {
				for (const item of this.itemList) {
					if (item.name == this.name) {
						return item;
					}
				}
				return null;
			}
		},
		methods: {
			async fetchConfig() {
				await this.handleSubmit(async () => {
					const containingPath = this.pathList.slice(0, -1);
					this.itemList = await this.$cache.get("list", ...containingPath);
				});
			}
		}
	};
</script>
