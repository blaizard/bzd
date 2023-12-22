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
				errors: [],
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
				}
			},
		},
		methods: {
			formatTopics(topics) {
				return topics.join("::");
			},
			filterLogs(logs) {
				return logs.filter((log) => {
					return this.showOnlyLevels.has(log.level);
				});
			},
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.$api.request("get", "/admin/logger/memory");
					this.errors = result.errors;
					this.logs = result.logs;
				});
			},
		},
	};
</script>
