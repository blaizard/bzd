<template>
	<div :class="{ accordion: true, show: state.showContent }">
		<template v-if="state.showAction">
			<i class="arrow arrow-down" @click="state.showContent = false" v-if="state.showContent"></i>
			<i class="arrow arrow-left" @click="state.showContent = true" v-else></i>
		</template>
		<span class="content" ref="content"><slot></slot></span>
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

<style lang="scss" scoped>
	.accordion {
		display: flex;
		flex-direction: row;
		align-items: baseline;

		.arrow {
			cursor: pointer;

			&:before {
				display: inline-block;
				width: 0;
				height: 0;
				border-left: 0.5em solid currentColor;
				border-right: 0.5em solid transparent;
				border-bottom: 0.5em solid transparent;
				border-top: 0.5em solid transparent;
				content: "";
				transition: transform 0.5s;
			}

			&.arrow-down:before {
				transform: rotate(90deg) translateY(0.25em) translateX(0.25em);
			}
		}

		.content {
			display: -webkit-box;
			-webkit-box-orient: vertical;
			overflow: hidden;
			-webkit-line-clamp: 1;
		}

		&.show {
			.content {
				-webkit-line-clamp: none;
			}
		}
	}
</style>
