<template>
	<div>
		<Table :value="users" :description="descriptionTable"></Table>
		<!--
		<tr v-for="data, uid in users">
			<td>{{ uid }}</td>
			<td v-for="key, value in data">{{ key }}</td>
			<td>
				<Button content="Delete" action="danger" @click="handleDelete(uid)"></Button>
			</td>
		</tr>
		//-->
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Button from "#bzd/nodejs/vue/components/form/element/button.vue";
	import Table from "#bzd/nodejs/vue/components/form/element/table.vue";

	export default {
		mixins: [Component],
		components: {
			Button,
			Table,
		},
		data: function () {
			return {
				users: [],
				page: 0,
				users: [
					{ uid: "hello", key: 2 },
					{ uid: "yes", price: "mo" },
				],
				next: null,
			};
		},
		props: {
			maxPerPage: { mandatory: false, type: String, default: 50 },
		},
		mounted() {
			this.handleFetch();
		},
		computed: {
			descriptionTable() {
				return {
					template: [
						{ type: "Input", caption: "UID", name: "uid", disable: true },
						{ type: "Date", caption: "Creation", name: "creation" },
						{ type: "Input", caption: "Roles", name: "roles", multi: true },
						{ type: "Button", content: "Delete", action: "danger" },
					],
				};
			},
		},
		methods: {
			async apiGetUsers() {
				return await this.$api.request("get", "/admin/users", { max: this.maxPerPage, page: parseInt(this.page) });
			},
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.apiGetUsers();
					let data = [];
					for (const [key, value] of Object.entries(result.data)) {
						data.push({
							uid: key,
							...value,
						});
					}
					this.users = data;
					this.next = result.next;
				});
			},
			async handleDelete(uid) {
				console.log(uid);
			},
		},
	};
</script>
