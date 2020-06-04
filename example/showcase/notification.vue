<template>
	<div>
        <button @click="addInfo">Info</button>
        <button @click="addError">Error</button>
        <button @click="addSuccess">Success</button>
	</div>
</template>

<script>
export default {
	methods: {
		addInfo() {
			this.$notification.info("Hello, I am an informative message!");
		},
		addError() {
			this.$notification.error("Hello, I am an error message!");
		},
		addSuccess() {
			this.$notification.success("Hello, I am a success message!");
		}
	}
};
</script>

<style lang="scss" scoped>
</style>
