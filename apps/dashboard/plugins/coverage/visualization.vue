<template>
	<div>
		<div class="coverage">
			<div class="lines" v-tooltip="tooltipCoverageLines">
				<span class="bzd-icon-lines"></span>
				{{ (metadata.coverage_lines * 100).toFixed(1) }}<small>%</small>
			</div>
			<div class="functions" v-if="hasCoverageFunctions" v-tooltip="tooltipCoverageFunctions">
				<small>Fct</small>
				{{ (metadata.coverage_functions * 100).toFixed(1) }}<small>%</small>
			</div>
			<div class="branches" v-if="hasCoverageBranches" v-tooltip="tooltipCoverageBranches">
				<small>Bch</small>
				{{ (metadata.coverage_branches * 100).toFixed(1) }}<small>%</small>
			</div>
		</div>
		<div class="metrics">{{ metadata.files }} <small>files</small> / {{ metadata.lines }} <small>lines</small></div>
	</div>
</template>

<script>
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";

	export default {
		props: {
			metadata: { type: Object, mandatory: true },
			description: { type: Object, mandatory: true },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		mounted() {},
		data: function () {
			return {};
		},
		watch: {
			metadata: {
				immediate: true,
				handler() {
					if ("link" in this.metadata) {
						const url = new URL(this.metadata.link, this.description["coverage.url"]);
						this.$emit("link", String(url));
					}
				},
			},
		},
		computed: {
			hasCoverageFunctions() {
				return "coverage_functions" in this.metadata;
			},
			hasCoverageBranches() {
				return "coverage_functions" in this.metadata;
			},
			tooltipCoverageLines() {
				return { data: "Line coverage over " + this.metadata.lines + " lines." };
			},
			tooltipCoverageFunctions() {
				return { data: "Function coverage over " + this.metadata.functions + " functions." };
			},
			tooltipCoverageBranches() {
				return { data: "Branch coverage over " + this.metadata.branches + " branches." };
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" as icons with ($bzdIconNames: link);

	.bzd-icon-lines {
		@include icons.defineIcon("lines.svg");
	}
</style>

<style lang="scss" scoped>
	.coverage {
		height: 200px;
		text-align: center;
		display: flex;
		justify-content: center;
		align-items: center;
		flex-direction: column;

		.lines {
			display: inline-block;
			font-size: 48px;
		}

		.functions,
		.branches {
			font-size: 24px;
			font-variant: small-caps;
		}
	}
	.metrics {
		text-align: center;
		height: 50px;
		line-height: 50px;
		font-size: 20px;
	}
</style>
