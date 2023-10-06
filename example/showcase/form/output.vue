<template>
	<div>
		<div class="bzd-showcase-form-element">
			<div>
				<h2>Form (default)</h2>
				<Form v-model="valueDefault" :description="formDescription"></Form>
			</div>
			<pre>{{ JSON.stringify(valueDefault, null, 4) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<div>
				<h2>Form (all=true)</h2>
				<Form v-model="valueAll" :description="formDescription" :all="true"></Form>
			</div>
			<pre>{{ JSON.stringify(valueAll, null, 4) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<div>
				<h2>Form (diff=true)</h2>
				<Form
					:value="valueDiff"
					@input="(v) => (valueDiffOutput = v)"
					:description="formDescription"
					:diff="true"
				></Form>
			</div>
			<pre>{{ JSON.stringify(valueDiffOutput, null, 4) }}</pre>
		</div>
	</div>
</template>

<script>
	import Template from "./template.vue";

	export default {
		mixins: [Template],
		data: function () {
			return {
				valueDefault: {
					input: "Hello",
					input2: "World",
				},
				valueAll: {
					input: "Hello",
					input2: "World",
				},
				valueDiff: {
					input: "Hello",
					input2: "World",
				},
				valueDiffOutput: {},
			};
		},
		computed: {
			formDescription() {
				return [
					{ type: "Input", name: "input", caption: "Input (named)" },
					{ type: "Input", name: "input2", caption: "Input 2 (named)" },
					{ type: "Input", caption: "Input (unmaned)" },
				];
			},
		},
	};
</script>
