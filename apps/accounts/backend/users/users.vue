<template>
	<div>
		<Modal v-model="showCreate">
			<span>Create a new user:</span>
			<Form :description="descriptionAdd" @submit="handleCreate"></Form>
		</Modal>
		<Button action="approve" content="Create" @click="showCreate = true"></Button>
		<Button
			v-if="hasChanges"
			:content="changesContent"
			action="approve"
			v-tooltip="changesTooltip"
			@click="handleApply"
		></Button>
		<Table v-model="users" :description="descriptionTable"></Table>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Button from "#bzd/nodejs/vue/components/form/element/button.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Table from "#bzd/nodejs/vue/components/form/element/table.vue";
	import Modal from "#bzd/nodejs/vue/components/modal/modal.vue";
	import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		mixins: [Component],
		components: {
			Button,
			Table,
			Form,
			Modal,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				users: [],
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
				return "Apply (" + this.nbChanges + ")";
			},
			descriptionTable() {
				return {
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
									name: "product",
									caption: "Product",
									list: {
										screen_recorder: "Screen Recorder",
									},
									width: 0.5,
								},
								{ type: "Date", caption: "End Date", name: "end", width: 0.5 },
							],
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
				};
			},
			descriptionAdd() {
				return [
					{ type: "Input", placeholder: "email", name: "uid", validation: "mandatory", width: 0.99 },
					{ type: "Button", content: "Create", action: "approve", width: 0.01 },
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
							this.users.push({
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
					this.users.push(values);
					this.showCreate = false;
				});
			},
			async handleDelete(index) {
				const user = this.users[index];
				const uid = user.uid;
				this.deletions.push(uid);
				// Remove existing updates on this UID if any.
				if (uid in this.updates) {
					this.$delete(this.updates, uid);
				}
				// Remove the element from the users list.
				this.users.splice(index, 1);
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
