<template>
	<div class="bzd-modal-wrapper" @click="handleCancel" v-if="modelValue">
		<div class="bzd-modal" @click.stop="">
			<slot></slot>
		</div>
	</div>
</template>

<script>
	export default {
		props: {
			// Used to show/hide the modal.
			modelValue: { type: Boolean, required: true },
		},
		emits: ["update:model-value"],
		methods: {
			handleCancel() {
				this.$emit("update:model-value", false);
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;

	.bzd-modal-wrapper {
		position: fixed;
		top: 0;
		left: 0;
		width: 100vw;
		height: 100vh;
		z-index: 10;
		overflow: auto;
		padding: 20px;
		background-color: rgba(0, 0, 0, 0.4);

		display: flex;
		flex-wrap: wrap;
		justify-content: center;
		align-items: center;

		.bzd-modal {
			min-width: 50%;
			background-color: config.$bzdGraphColorWhite;
			padding: 25px;
			border-radius: 5px;
		}
	}
</style>
