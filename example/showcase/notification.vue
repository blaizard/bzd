<template>
	<div>
        <button @click="addInfo">Info</button>
        <button @click="addError">Error</button>
        <button @click="addSuccess">Success</button>
        <button @click="increaseCounter">Counter</button>
	</div>
</template>

<script>
export default {
	data() {
		return {
			counter: 0
		};
	},
	methods: {
		addInfo() {
			this.$notification.info("Hello, I am an informative message!");
		},
		addError() {
			this.$notification.error("Hello, I am an error message!");
		},
		addSuccess() {
			this.$notification.success("Hello, I am a success message!");
		},
		increaseCounter() {
			this.$notification.success({"counter": "Counter: " + (++this.counter)});
		}
	}
};
</script>

<style lang="scss" scoped>
</style>
