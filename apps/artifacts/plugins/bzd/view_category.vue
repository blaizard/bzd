<template>
	<pre><code>{{ formatedMetadata }}</code></pre>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import HttpClient from "#bzd/nodejs/core/http/client.mjs";

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
					const endpoint = "/x/" + this.pathList.map(encodeURIComponent).join("/");
					this.metadata = await HttpClient.get(endpoint, {
						expect: "json",
					});
				});
			},
		},
	};
</script>
