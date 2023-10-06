<template>
	<div>
		<Button v-if="hasChanges" :content="changesContent" action="approve" v-tooltip="changesTooltip"></Button>
		<Table v-model="users" :description="descriptionTable"></Table>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Button from "#bzd/nodejs/vue/components/form/element/button.vue";
	import Table from "#bzd/nodejs/vue/components/form/element/table.vue";
	import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
	import { objectDifference } from "#bzd/nodejs/utils/object.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		mixins: [Component],
		components: {
			Button,
			Table,
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
						{ type: "Input", caption: "UID", name: "uid", editable: false },
						{ type: "Date", caption: "Creation", name: "creation" },
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
		},
	};
</script>
