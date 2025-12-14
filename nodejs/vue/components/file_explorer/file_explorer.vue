<template>
	<div class="file-explorer">
		<Tree :fetch="fetch" :show-path="path" @item="handleEventPropagation('item', $event)"></Tree>
	</div>
</template>

<script>
	import Tree from "#bzd/nodejs/vue/components/file_explorer/tree.vue";

	export default {
		components: {
			Tree,
		},
		props: {
			path: { type: Array, mandatory: false, default: () => [] },
			fetch: { type: Function, mandatory: true },
		},
		emits: ["item"],
		methods: {
			handleEventPropagation(name, event) {
				this.$emit(name, event);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.file-explorer {
		padding: 10px;
	}
</style>
