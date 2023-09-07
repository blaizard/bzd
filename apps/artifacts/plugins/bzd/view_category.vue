<template>
	<pre><code>{{ formatedMetadata }}</code></pre>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";

	export default {
		mixins: [Base, Component],
		data: function () {
			return {
				metadata: {},
			};
		},
		computed: {
			formatedMetadata() {
				return JSON.stringify(this.metadata, null, 4);
			},
		},
		mounted() {
			this.fetchMetadata();
		},
		methods: {
			async fetchMetadata() {
				await this.handleSubmit(async () => {
					this.metadata = await this.$api.request("get", "/endpoint/{path:*}", {
						path: this.pathList.map(encodeURIComponent).join("/"),
					});
				});
			},
		},
	};
</script>
