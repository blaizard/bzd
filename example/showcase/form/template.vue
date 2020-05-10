<template>
	<div>
        <h1>Form</h1>
        <Form v-model="value" :description="formDescription"></Form>
        <h1>Values</h1>
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
                value: {}
			}
		},
        computed: {
            formDescription() {
                return []
            },
        }
	}
</script>
