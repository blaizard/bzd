<template>
	<div class="value-container">
		<a class="more" @click="viewMore" v-tooltip="tooltipActionMore">...</a>
		<div class="value-grid">
			<div v-for="[t, v] in valueTimestampList" class="item">
				<span class="timestamp">{{ timestampToString(t) }}</span
				><span class="value"><slot :value="v"></slot></span>
			</div>
		</div>
	</div>
</template>

<script>
	import { dateToString } from "#bzd/nodejs/utils/to_string.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import HttpClient from "#bzd/nodejs/core/http/client.mjs";

	export default {
		mixins: [Component],
		props: {
			value: { mandatory: true },
			pathList: { mandatory: true, type: Array },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				count: 1,
				timeout: null,
				metadata: null,
			};
		},
		beforeUnmount() {
			clearTimeout(this.timeout);
		},
		computed: {
			valueTimestampList() {
				if (this.metadata) {
					return [this.metadata.data];
				}
				return [this.value];
			},
			tooltipActionMore() {
				return {
					type: "text",
					data: "View more...",
				};
			},
		},
		methods: {
			async viewMore() {
				this.count = Math.max(5, this.count * 2);
				await this.fetchMetadata();
			},
			timestampToString(timestamp) {
				return dateToString("{y}-{m}-{d} {h}:{min}:{s}", timestamp);
			},
			async fetchMetadata() {
				if (this.count > 1) {
					await this.handleSubmit(async () => {
						const endpoint = "/x/" + this.pathList.map(encodeURIComponent).join("/");
						this.metadata = await HttpClient.get(endpoint, {
							query: {
								last: this.count,
							},
							expect: "json",
						});
					});
					this.timeout = setTimeout(this.fetchMetadata, 1000);
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	.value-container {
		display: inline-flex;

		a.more {
			background-color: #eee;
			border: solid 1px rgb(229.5, 229.5, 229.5);
			border-radius: 6px;
			padding: 2px 5px;
			font-size: 70%;
			margin-right: 5px;
			font-weight: bold;
			align-self: baseline;
		}

		.value-grid {
			display: inline-grid;
			align-self: baseline;

			.item {
				display: flex;

				.timestamp {
					background-color: #eee;
					border: solid 1px rgb(229.5, 229.5, 229.5);
					border-radius: 6px;
					padding: 2px 5px;
					font-size: 70%;
					margin-right: 5px;
					opacity: 70%;
					align-self: baseline;
					white-space: nowrap;
				}

				.value {
					align-self: baseline;
				}
			}
		}
	}
</style>
