<template>
	<div :class="{ accordion: true, show: state.show }">
		<i class="bzd-icon-arrow_up action" @click="state.show = false" v-if="state.show"></i>
		<i class="bzd-icon-arrow_down action" @click="state.show = true" v-else></i>
		<span class="content"><slot></slot></span>
	</div>
</template>

<script setup>
	import { reactive } from "vue";
	const state = reactive({
		show: false,
	});
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
