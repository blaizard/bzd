<template>
	<div>
		<a @click="handleList">List!</a>
		<div>{{ response }}</div>
	</div>
</template>

<script>
	export default {
		data: function () {
			return {
				response: null
			};
		},
		methods: {
			async handleList() {
				this.response = await this.$api.request("get", "/list", {
					path: "/"
				});
			}
		}
	}
</script>
