<template>
	<div class="router">
		<div>Clicked on: {{ clicked }}</div>
		<RouterLink link="/link1">RouterLink -> /link1</RouterLink>
	</div>
</template>

<script>
	export default {
		data: function () {
			return {
				clicked: null,
			};
		},
		mounted() {
			this.$router.set({
				routes: [
					{
						path: "/link1",
						handler: () => {
							this.clicked = "/link1";
						},
					},
					{
						path: "/hello",
					},
				],
			});
		},
	};
</script>

<style lang="scss" scoped>
	.router {
	}
</style>
