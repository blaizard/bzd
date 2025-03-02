<template>
	<div>
		<i v-if="imageUrl" class="icon" :style="style"></i>
	</div>
</template>

<script>
	export default {
		props: {
			description: { type: Object, mandatory: true },
			color: { type: String, mandatory: true },
		},
		computed: {
			imageUrl() {
				return this.description["link.image"] || null;
			},
			style() {
				return {
					"background-color": this.color,
					"-webkit-mask-image": "url(" + this.imageUrl + ")",
					"mask-image": "url(" + this.imageUrl + ")",
				};
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as * with (
		$bzdIconNames: link
	);
</style>

<style lang="scss" scoped>
	.icon {
		-webkit-mask-size: cover;
		mask-size: contain;
		mask-repeat: no-repeat;
		mask-position: center;
		position: absolute;
		top: 15%;
		left: 15%;
		width: 70%;
		height: 70%;
		display: block;
	}
</style>
