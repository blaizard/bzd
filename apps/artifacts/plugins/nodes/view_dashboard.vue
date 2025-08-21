<template>
	<div class="data">
		<div style="width: 800px"><canvas id="acquisitions"></canvas></div>
		{{ dashboards }}
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Chart from "chart.js/auto";

	const data = [
		{ year: 2010, count: 10 },
		{ year: 2011, count: 20 },
		{ year: 2012, count: 15 },
		{ year: 2013, count: 25 },
		{ year: 2014, count: 22 },
		{ year: 2015, count: 30 },
		{ year: 2016, count: 28 },
	];

	export default {
		mixins: [Base, Component],
		data: function () {
			return {
				dashboards: {},
			};
		},
		mounted() {
			this.fetchDashboards();

			const char = new Chart(document.getElementById("acquisitions"), {
				type: "line",
				data: {
					labels: data.map((row) => row.year),
					datasets: [
						{
							label: "Acquisitions by year",
							data: data.map((row) => row.count),
						},
					],
				},
			});
		},
		computed: {
			dashboardEndpoint() {
				const [volume, ...rest] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@dashboards/" + rest.map(encodeURIComponent).join("/");
			},
		},
		methods: {
			async fetchDashboards() {
				await this.handleSubmit(async () => {
					this.dashboards = await this.requestBackend(this.dashboardEndpoint, {
						method: "get",
						expect: "json",
					});
				});
			},
		},
	};
</script>

<style lang="scss" scoped></style>
