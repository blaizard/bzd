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
                <Form v-model="valueDiff" :description="formDescription" :diff="true"></Form>
            </div>
            <pre>{{ JSON.stringify(valueDiff, null, 4) }}</pre>
        </div>
	</div>
</template>

<script>
    import Template from "./template.vue"; 

	export default {
        mixins: [Template],
		data: function () {
			return {
                valueDefault: {},
                valueAll: {},
                valueDiff: {
                    input: "Hello"
                }
			}
		},
        computed: {
            formDescription() {
                return [
                    { type: "Input", name: "input", caption: "Input (named)" },
                    { type: "Input", caption: "Input (unmaned)" },
                ]
            },
        }
	}
</script>
