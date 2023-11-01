<template>
	<div>
		<Form v-model="user" :description="description" @input-with-context="handleChange"></Form>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		mixins: [Component],
		components: {
			Form,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				user: {},
				updates: {},
			};
		},
		mounted() {
			this.handleFetch();
		},
		computed: {
			nbChanges() {
				return Object.keys(this.updates).length;
			},
			hasChanges() {
				return this.nbChanges > 0;
			},
			changesContent() {
				return this.hasChanges ? "Apply (" + this.nbChanges + ")" : "Apply";
			},
			roles() {
				return this.user.roles || [];
			},
			description() {
				return [
					{
						type: "Button",
						action: "danger",
						content: this.changesContent,
						disable: !this.hasChanges,
						click: this.handleApply,
						tooltip: this.changesTooltip,
						align: "right",
					},
					{ type: "Input", caption: "Email", name: "email", disable: true },
					{ type: "Date", caption: "Creation", name: "creation", disable: true, width: 0.3 },
					{ type: "Date", caption: "Last Login", name: "last_login", disable: true, width: 0.3 },
					{ type: "Input", caption: "Roles", name: "roles", multi: true, disable: true, width: 0.4 },
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
					{
						type: "Array",
						caption: "Tokens",
						name: "tokens",
						template: [
							{
								type: "Input",
								name: "key",
								caption: "Token",
								disable: true,
								width: 0.3,
							},
							{ type: "Date", caption: "Expiration", name: "expiration", width: 0.3 },
							{
								type: "Dropdown",
								caption: "Roles",
								name: "roles",
								multi: true,
								list: this.roles,
								width: 0.3,
							},
						],
						allowAdd: false,
						toInputValue: "map_to_list",
						toOutputValue: "list_to_map",
					},
				];
			},
			changesTooltip() {
				return {
					data: this.hasChanges ? "<h3>Updates</h3>" + Object.keys(this.updates).join("<br/>") : "",
				};
			},
		},
		methods: {
			async handleFetch() {
				await this.handleSubmit(async () => {
					this.user = await this.$api.request("get", "/user");
				});
			},
			handleChange(data) {
				const name = data.context.name;
				this.$set(this.updates, name, this.user[name]);
			},
			async handleApply() {
				await this.handleSubmit(async () => {
					await this.$api.request("put", "/user", {
						...this.updates,
					});
				});
				this.updates = {};
			},
		},
	};
</script>
