<template>
	<div class="router">
		<div>Clicked on: {{ clicked }}</div>
		<div><a href="#/link1">href link -> /link1</a></div>
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
			this.$routerSet({
				routes: [
					{
						path: "/link1",
						handler: () => {
							this.clicked = "/link1";
						},
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
