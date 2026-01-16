<template>
	<div>
		<h1>Form</h1>
		<Form
			v-model="value"
			:description="updatedFormDescription"
			@submit="handleSumbit"
			@input="handleInput"
			:all="true"
		></Form>
		<h1>Values</h1>
		<code v-if="submitted">Submitted</code>
		<pre>{{ JSON.stringify(value, jsonReplacer, 4) }}</pre>
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
			updatedFormDescription() {
				return this.formDescription.concat([{ type: "Button", action: "approve" }]);
			},
		},
		methods: {
			handleSumbit(values) {
				this.submitted = true;
				console.log("values.getFormData() =", values.getFormData());
			},
			handleInput() {
				this.submitted = false;
			},
			jsonReplacer(key, value) {
				const c = value.constructor;
				const hasConstructor = !(c === Object || c === Array || c === Number || c === String || c === Boolean);
				if (hasConstructor) {
					return "<" + c.name + ">";
				}
				return value;
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
