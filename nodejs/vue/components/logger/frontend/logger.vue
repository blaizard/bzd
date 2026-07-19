<template>
	<div>
		<Form v-model="form" :description="description"></Form>
		<table>
			<tr>
				<th>Time</th>
				<th>Level</th>
				<th>Topics</th>
				<th>Message</th>
			</tr>
			<tr v-for="log in filterLogs(logs)">
				<td>{{ log.date }}</td>
				<td>{{ log.level }}</td>
				<td>{{ formatTopics(log.topics) }}</td>
				<td>{{ log.message }}</td>
			</tr>
		</table>
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
		mounted() {
			this.handleFetch();
		},
		data: function () {
			return {
				logs: [],
				form: {
					minLevel: "info",
				},
			};
		},
		computed: {
			description() {
				return [
					{
						type: "Dropdown",
						name: "minLevel",
						caption: "Level",
						list: ["debug", "info", "warning", "error"],
						width: 0.1,
					},
				];
			},
			showOnlyLevels() {
				switch (this.form.minLevel) {
					case "debug":
						return new Set(["debug", "info", "warning", "error"]);
					case "info":
						return new Set(["info", "warning", "error"]);
					case "warning":
						return new Set(["warning", "error"]);
					case "error":
						return new Set(["error"]);
					default:
						return new Set();
				}
			},
		},
		methods: {
			formatTopics(topics) {
				if (Array.isArray(topics)) {
					return topics.join("::");
				}
				return "-";
			},
			filterLogs(logs) {
				return logs.filter((log) => {
					return this.showOnlyLevels.has(log.level);
				});
			},
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/logger");
					const merged = [];
					for (const entries of Object.values(result.logs)) {
						for (const entry of entries) {
							merged.push(entry);
						}
					}
					merged.sort((a, b) => (a.date < b.date ? -1 : a.date > b.date ? 1 : 0));
					this.logs = merged;
				});
			},
		},
	};
</script>
