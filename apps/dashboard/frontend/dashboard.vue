<template>
	<div>Dashboard</div>
</template>

<script>
	"use strict"

	export default {
		data: function () {
			return {
			}
		},
		mounted() {
			this.$registerRouter({
				routes: [
                    { path: '/edit', handler: () => { console.log("What!!!"); alert("What!!!"); } },
				],
			});
		},
	}
</script>
