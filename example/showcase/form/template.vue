<template>
	<div>
        <h1>Form</h1>
        <Form v-model="value" :description="formDescription" @submit="handleSumbit" @input="handleInput"></Form>
        <h1>Values</h1>
        <code v-if="submitted">Submitted</code>
        <pre>{{ JSON.stringify(value, null, 4) }}</pre>
	</div>
</template>

<script>
    import Form from "bzd/vue/components/form/form.vue"; 

	export default {
        components: {
            Form
        },
		data: function () {
			return {
                value: {},
                submitted: false
			}
		},
        computed: {
            formDescription() {
                return []
            },
        },
        methods: {
            handleSumbit() {
                this.submitted = true;
            },
            handleInput() {
                this.submitted = false;
            }
        }
	}
</script>
