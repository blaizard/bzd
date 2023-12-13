<template>
	<div>
		<Modal v-model="showCreate">
			<span>Create a new application:</span>
			<Form :description="descriptionAdd" @submit="handleCreate"></Form>
		</Modal>
		<Form v-model="form" :description="description"></Form>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Modal from "#bzd/nodejs/vue/components/modal/modal.vue";
	import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import { allScopes } from "#bzd/apps/accounts/backend/users/scopes.mjs";

	export default {
		mixins: [Component],
		components: {
			Form,
			Modal,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				form: {
					applications: [],
				},
				deletions: [],
				showCreate: false,
			};
		},
		props: {
			maxPerPage: { mandatory: false, type: Number, default: 50 },
			fetchChunk: { mandatory: false, type: Number, default: 1000 },
		},
		mounted() {
			this.handleFetch();
		},
		computed: {
			hasDeletions() {
				return this.deletions.length > 0;
			},
			hasChanges() {
				return this.hasDeletions;
			},
			nbChanges() {
				return this.deletions.length;
			},
			changesContent() {
				return this.hasChanges ? "Apply (" + this.nbChanges + ")" : "Apply";
			},
			description() {
				return [
					{
						type: "Button",
						action: "approve",
						content: "Create",
						click: () => {
							this.showCreate = true;
						},
						align: "right",
						width: 0.99,
					},
					{
						type: "Button",
						action: "danger",
						content: this.changesContent,
						disable: !this.hasChanges,
						click: this.handleApply,
						tooltip: this.changesTooltip,
						align: "right",
						width: 0.01,
					},
					{
						type: "Table",
						name: "applications",
						template: [
							{ type: "Input", caption: "UID", name: "uid", disable: true },
							{ type: "Date", caption: "Creation", name: "creation", disable: true },
							{ type: "Input", caption: "Redirect", name: "redirect", disable: true },
							{
								type: "Dropdown",
								caption: "Scopes",
								name: "scopes",
								multi: true,
								disable: true,
							},
							{
								type: "Button",
								content: "Delete",
								action: "danger",
								click: (context) => {
									this.handleDelete(context.row);
								},
							},
						],
						onchange: (value, context) => {
							const valueRow = value[context.row];
							const uid = valueRow.uid;
							this.updates[uid] = {
								...(this.updates[uid] || {}),
								[context.name]: valueRow[context.name],
							};
						},
					},
				];
			},
			descriptionAdd() {
				return [
					{ type: "Input", placeholder: "Identifier", name: "uid", validation: "mandatory", width: 0.3 },
					{ type: "Input", placeholder: "Redirect URL", name: "redirect", validation: "mandatory", width: 0.4 },
					{
						type: "Dropdown",
						placeholder: "Scopes",
						name: "scopes",
						multi: true,
						list: allScopes.toList(),
						width: 0.3,
					},
					{ type: "Button", content: "Create", action: "danger" },
				];
			},
			changesTooltip() {
				const deletions = this.hasDeletions
					? "<h3>Delete</h3>" +
						this.deletions
							.map((uid) => {
								return "<b>" + uid + "</b>";
							})
							.join("<br/>")
					: "";

				return {
					data: deletions,
				};
			},
		},
		methods: {
			async handleFetch() {
				await this.handleSubmit(async () => {
					let paging = CollectionPaging.pagingFromParam(this.fetchChunk);
					do {
						const result = await this.$api.request("get", "/admin/applications", paging);
						paging = result.next;
						for (const [key, value] of Object.entries(result.data)) {
							this.form.applications.push({
								uid: key,
								...value,
							});
						}
					} while (paging);
				});
			},
			async handleCreate(values) {
				await this.handleSubmit(async () => {
					await this.$api.request("post", "/admin/application", values);
					this.form.applications.push(values);
					this.showCreate = false;
				});
			},
			async handleDelete(index) {
				const application = this.form.applications[index];
				const uid = application.uid;
				this.deletions.push(uid);
				this.form.applications.splice(index, 1);
			},
			async handleApply() {
				// Process the deletion.
				while (this.hasDeletions) {
					const uid = this.deletions[0];
					await this.handleSubmit(async () => {
						await this.$api.request("delete", "/admin/application", {
							uid: uid,
						});
					});
					this.deletions.shift();
				}
			},
		},
	};
</script>
