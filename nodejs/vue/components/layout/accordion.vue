<template>
	<div :class="{ accordion: true, show: state.showContent }">
		<template v-if="state.showAction">
			<i class="bzd-icon-arrow_up action" @click="state.showContent = false" v-if="state.showContent"></i>
			<i class="bzd-icon-arrow_down action" @click="state.showContent = true" v-else></i>
		</template>
		<span class="content" ref="content" @click="isOverflow"><slot></slot></span>
	</div>
</template>

<script setup>
	import { ref, reactive, onMounted, onUnmounted } from "vue";

	const state = reactive({
		showContent: false,
		showAction: false,
	});
	const content = ref(null);
	let observer = null;

	onMounted(() => {
		observer = new ResizeObserver((e) => {
			if (!state.showContent) {
				state.showAction = isOverflow(e[0].target);
			}
		}).observe(content.value);
	});

	onUnmounted(() => {
		if (observer) {
			observer.unobserve(content.value);
		}
	});

	const isOverflow = (element) => {
		return element.scrollHeight > element.offsetHeight;
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as * with (
		$bzdIconNames: arrow_down arrow_up
	);
</style>

<style lang="scss" scoped>
	@use "@/nodejs/styles/default/css/clickable.scss" as *;

	.accordion {
		display: flex;
		flex-direction: row;
		align-items: baseline;
		gap: 10px;

		.action {
			@extend %bzd-clickable;
			font-weight: bold;
		}

		.content {
			display: -webkit-box;
			-webkit-box-orient: vertical;
			overflow: hidden;
			-webkit-line-clamp: 1;
			white-space: pre;
		}

		&.show {
			.content {
				-webkit-line-clamp: none;
			}
		}
	}
</style>
