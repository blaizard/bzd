<template>
	<div class="value-container">
		<div v-for="[t, v] in valueTimestampList" class="inline">
			<span>{{ t }}</span
			><code><slot class="value" :value="v"></slot></code>
		</div>
	</div>
</template>

<script>
	export default {
		props: {
			value: { mandatory: true },
			pathList: { mandatory: true, type: Array },
		},
		computed: {
			valueTimestampList() {
				return [this.value];
			},
		},
		methods: {},
	};
</script>

<style lang="scss" scoped>
	.value-container {
		display: inline;

		.inline {
			display: inline;
		}

		.value {
			background-color: #ccc;
			border: solid 1px rgb(229.5, 229.5, 229.5);
			border-radius: 6px;
			padding: 2px 5px;
		}
	}
</style>
