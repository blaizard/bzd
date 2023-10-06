<template>
	<div>
		<Form :description="descriptionAdd"></Form>
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
	import { objectDifference } from "#bzd/nodejs/utils/object.mjs";
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
			};
		},
		props: {
			maxPerPage: { mandatory: false, type: String, default: 50 },
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
						{ type: "Input", caption: "Roles", name: "roles", multi: true },
						{ type: "Button", content: "Delete", action: "danger" },
					],
					onchange: (value, oldValue, index) => {
						const rowValue = value[index];
						const uid = rowValue.uid;
						// Note the order here is important.
						const diff = objectDifference(oldValue[index] || {}, rowValue);
						this.$set(this.updates, uid, Object.assign({}, this.updates[uid] || {}, diff));
					},
				};
			},
			descriptionAdd() {
				return [
					{ type: "Input", placeholder: "email", name: "uid", width: 0.9 },
					{ type: "Button", content: "Create", action: "approve", width: 0.1 },
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
					let paging = CollectionPaging.pagingFromParam(this.maxPerPage);
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
			async handleDelete(uid) {
				console.log(uid);
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
			},
		},
	};
</script>
