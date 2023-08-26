<template>
	<div>
		<h1>Form</h1>
		<Form v-model="value" :description="formDescription" @submit="handleSumbit" @input="handleInput" :all="true"></Form>
		<h1>Values</h1>
		<code v-if="submitted">Submitted</code>
		<pre>{{ JSON.stringify(value, null, 4) }}</pre>
	</div>
</template>

<script>
	import Form from "#bzd/nodejs/vue/components/form/form.vue";

	export default {
		components: {
			Form,
		},
		data: function () {
			return {
				value: {},
				submitted: false,
			};
		},
		computed: {
			formDescription() {
				return [];
			},
		},
		methods: {
			handleSumbit() {
				this.submitted = true;
			},
			handleInput() {
				this.submitted = false;
			},
		},
	};
</script>

<style lang="scss">
	.bzd-showcase-form-element {
		display: flex;
		flex-flow: row wrap;
		> * {
			margin: 5px 20px;
			flex: 1;
		}
	}
</style>
