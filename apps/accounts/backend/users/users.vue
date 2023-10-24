<template>
	<div>
		<Modal v-model="showCreate">
			<span>Create a new user:</span>
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
					users: [],
				},
				deletions: [],
				updates: {},
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
			hasUpdates() {
				return Object.keys(this.updates).length > 0;
			},
			hasDeletions() {
				return this.deletions.length > 0;
			},
			hasChanges() {
				return this.hasUpdates || this.hasDeletions;
			},
			nbChanges() {
				return (
					Object.values(this.updates).reduce((value, data) => value + Object.keys(data).length, 0) +
					this.deletions.length
				);
			},
			changesContent() {
				return this.hasChanges > 0 ? "Apply (" + this.nbChanges + ")" : "Apply";
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
						name: "users",
						template: [
							{ type: "Input", caption: "UID", name: "uid", disable: true },
							{ type: "Date", caption: "Creation", name: "creation", disable: true },
							{ type: "Date", caption: "Last Login", name: "last_login", disable: true },
							{ type: "Input", caption: "Roles", name: "roles", multi: true },
							{
								type: "Array",
								caption: "Subscriptions",
								name: "subscriptions",
								template: [
									{
										type: "Dropdown",
										name: "key",
										caption: "Product",
										list: {
											screen_recorder: "Screen Recorder",
										},
										width: 0.5,
									},
									{ type: "Date", caption: "End Date", name: "end", width: 0.5 },
								],
								toInputValue: "map_to_list",
								toOutputValue: "list_to_map",
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
							this.$set(this.updates, uid, {
								...(this.updates[uid] || {}),
								[context.name]: valueRow[context.name],
							});
						},
					},
				];
			},
			descriptionAdd() {
				return [
					{ type: "Input", placeholder: "email", name: "uid", validation: "mandatory", width: 0.99 },
					{ type: "Button", content: "Create", action: "danger", width: 0.01 },
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

				const updates = this.hasUpdates
					? "<h3>Update</h3>" +
					  Object.entries(this.updates)
							.map((entry) => {
								const [uid, data] = entry;
								return "<b>" + uid + "</b>: " + Object.keys(data);
							})
							.join("<br/>")
					: "";

				return {
					data: deletions + updates,
				};
			},
		},
		methods: {
			async handleFetch() {
				await this.handleSubmit(async () => {
					let paging = CollectionPaging.pagingFromParam(this.fetchChunk);
					do {
						const result = await this.$api.request("get", "/admin/users", paging);
						paging = result.next;
						for (const [key, value] of Object.entries(result.data)) {
							this.form.users.push({
								uid: key,
								...value,
							});
						}
					} while (paging);
				});
			},
			async handleCreate(values) {
				await this.handleSubmit(async () => {
					await this.$api.request("post", "/admin/user", values);
					this.form.users.push(values);
					this.showCreate = false;
				});
			},
			async handleDelete(index) {
				const user = this.form.users[index];
				const uid = user.uid;
				this.deletions.push(uid);
				// Remove existing updates on this UID if any.
				if (uid in this.updates) {
					this.$delete(this.updates, uid);
				}
				// Remove the element from the users list.
				this.form.users.splice(index, 1);
			},
			async handleApply() {
				// Process the updates.
				while (this.hasUpdates) {
					const [uid, values] = Object.entries(this.updates)[0];
					await this.handleSubmit(async () => {
						await this.$api.request("put", "/admin/user", {
							uid: uid,
							...values,
						});
					});
					this.$delete(this.updates, uid);
				}
				// Process the deletion.
				while (this.hasDeletions) {
					const uid = this.deletions[0];
					await this.handleSubmit(async () => {
						await this.$api.request("delete", "/admin/user", {
							uid: uid,
						});
					});
					this.deletions.shift();
				}
			},
		},
	};
</script>
