<template>
	<div>
		<Form v-model="user" :description="description"></Form>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";

	export default {
		mixins: [Component],
		components: {
			Form,
		},
		data: function () {
			return {
				user: {},
			};
		},
		mounted() {
			this.handleFetch();
		},
		computed: {
			description() {
				return [
					{ type: "Input", caption: "Email", name: "email", disable: true },
					{ type: "Date", caption: "Creation", name: "creation", disable: true, width: 0.5 },
					{ type: "Date", caption: "Last Login", name: "last_login", disable: true, width: 0.5 },
					{
						type: "Array",
						caption: "Subscriptions",
						name: "subscriptions",
						template: [
							{
								type: "Input",
								name: "key",
								caption: "Product",
								width: 0.5,
							},
							{ type: "Date", caption: "End Date", name: "end", width: 0.5 },
						],
						disable: true,
						hideAddWhenDisabled: true,
						toInputValue: "map_to_list",
					},
				];
			},
		},
		methods: {
			async handleFetch() {
				await this.handleSubmit(async () => {
					this.user = await this.$api.request("get", "/user");
				});
			},
		},
	};
</script>
