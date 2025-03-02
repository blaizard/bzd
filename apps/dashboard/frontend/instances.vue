<template>
	<div class="bzd-dashboard-instances">
		<template v-for="data in layout">
			<Instance v-if="data.type == 'tile'" :key="data.uid" :uid="data.uid" :description="data"></Instance>
			<div v-else-if="data.type == 'separator'" class="separator">{{ data.name }}</div>
		</template>
	</div>
</template>

<script>
	import Instance from "./instance.vue";

	export default {
		components: {
			Instance,
		},
		props: {
			another: { type: String, mandatory: false, default: null },
		},
		data: function () {
			return {
				layout: [],
			};
		},
		mounted() {
			this.getLayout();
		},
		methods: {
			async getLayout() {
				this.layout = (await this.$rest.request("get", "/layout")).layout;
			},
		},
	};
</script>

<style lang="scss">
	.bzd-dashboard-instances {
		display: flex;
		flex-flow: row wrap;

		.separator {
			flex-basis: 100%;
			font-weight: bold;
		}
	}
</style>
