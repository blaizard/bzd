<template>
	<div>
		<Form v-model="user" :description="description" @update-with-context="handleChange"></Form>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import {
		scopeSelfBasicRead,
		scopeSelfTokensRead,
		scopeSelfTokensWrite,
		scopeSelfSubscriptionsRead,
	} from "#bzd/apps/accounts/backend/users/scopes.mjs";

	export default {
		mixins: [Component],
		components: {
			Form,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		props: {
			uid: { type: String, mandatory: false, default: null },
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
			scopes() {
				return this.user.scopes || [];
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
					{
						type: "Input",
						caption: "Email",
						name: "email",
						disable: true,
						condition: Boolean(this.$authentication.hasScope(scopeSelfBasicRead)),
					},
					{
						type: "Date",
						caption: "Creation",
						name: "creation",
						disable: true,
						width: 0.2,
						condition: Boolean(this.$authentication.hasScope(scopeSelfBasicRead)),
					},
					{
						type: "Date",
						caption: "Last Login",
						name: "last_login",
						disable: true,
						width: 0.2,
						condition: Boolean(this.$authentication.hasScope(scopeSelfBasicRead)),
					},
					{
						type: "Date",
						caption: "Last Failed Login",
						name: "last_failed_login",
						disable: true,
						width: 0.2,
						condition: Boolean(this.$authentication.hasScope(scopeSelfBasicRead)),
					},
					{
						type: "Date",
						caption: "Last Password Reset Request",
						name: "last_password_reset",
						disable: true,
						width: 0.2,
						condition: Boolean(this.$authentication.hasScope(scopeSelfBasicRead)),
					},
					{
						type: "Input",
						caption: "Roles",
						name: "roles",
						multi: true,
						disable: true,
						width: 0.4,
						condition: Boolean(this.$authentication.hasScope(scopeSelfBasicRead)),
					},
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
						condition: Boolean(this.$authentication.hasScope(scopeSelfSubscriptionsRead)),
					},
					{
						type: "Array",
						caption: "Tokens",
						name: "tokens",
						template: [
							{
								type: "Input",
								name: "identifier",
								caption: "Identifier",
								disable: true,
								width: 0.2,
							},
							{
								type: "Input",
								name: "key",
								caption: "Token",
								disable: true,
								width: 0.2,
							},
							{ type: "Date", caption: "Creation", name: "creation", disable: true, width: 0.2 },
							{
								type: "Date",
								caption: "Expiration",
								name: "expiration",
								width: 0.2,
								disable: !Boolean(this.$authentication.hasScope(scopeSelfTokensWrite)),
							},
							{
								type: "Dropdown",
								caption: "Scopes",
								name: "scopes",
								multi: true,
								list: this.scopes,
								width: 0.2,
								disable: !Boolean(this.$authentication.hasScope(scopeSelfTokensWrite)),
							},
						],
						allowAdd: false,
						toInputValue: "map_to_list",
						toOutputValue: "list_to_map",
						condition: Boolean(this.$authentication.hasScope(scopeSelfTokensRead)),
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
					this.user = await this.$rest.request("get", "/user", this.uid ? { uid: this.uid } : {});
				});
			},
			handleChange(data) {
				const name = data.context.name;
				this.updates[name] = this.user[name];
			},
			async handleApply() {
				await this.handleSubmit(async () => {
					await this.$rest.request("put", "/user", {
						...this.updates,
						...(this.uid ? { uid: this.uid } : {}),
					});
				});
				this.updates = {};
			},
		},
	};
</script>
