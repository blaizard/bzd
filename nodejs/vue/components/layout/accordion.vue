<template>
	<div
		:class="{ accordion: true, show: state.showContent, overflow: state.overflow }"
		:style="{ '--nb-lines': props.lines }"
		@click="click"
	>
		<template v-if="state.showAction">
			<i class="arrow arrow-down" @click="state.showContent = false" v-if="state.showContent"></i>
			<i class="arrow arrow-left" v-else></i>
		</template>
		<span class="content" ref="content"><slot></slot></span>
	</div>
</template>

<script setup>
	import { ref, reactive, onMounted, onUnmounted } from "vue";

	const props = defineProps({
		lines: {
			type: Number,
			default: 1,
		},
	});
	const state = reactive({
		showContent: false,
		showAction: false,
		overflow: false,
	});
	const content = ref(null);
	let observer = null;

	onMounted(() => {
		observer = new ResizeObserver((e) => {
			// Debounce to avoid infinite loop.
			setTimeout(() => {
				state.overflow = isOverflow(e[0].target);
				if (!state.showContent) {
					state.showAction = state.overflow;
				}
			}, 1);
		}).observe(content.value);
	});

	onUnmounted(() => {
		if (observer) {
			observer.unobserve(content.value);
		}
	});

	const click = () => {
		if (state.overflow) {
			state.showContent = !state.showContent;
		}
	};

	const isOverflow = (element) => {
		return element.scrollHeight > element.offsetHeight;
	};
</script>

<style lang="scss" scoped>
	.accordion {
		display: flex;
		flex-direction: row;
		align-items: baseline;

		&.overflow {
			cursor: pointer;
		}

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
			-webkit-line-clamp: var(--nb-lines);
		}

		&.show {
			.content {
				-webkit-line-clamp: none;
			}
		}
	}
</style>
