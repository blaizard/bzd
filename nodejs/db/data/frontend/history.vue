<template>
	<div class="history">
		<div v-if="historyValue.length" ref="scroll" class="history-scroll" @scroll="onScroll">
			<Value :value="historyValue" :view="0" :timestamp="timestamp"></Value>
		</div>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Value from "#bzd/nodejs/db/data/frontend/value.vue";

	export default {
		mixins: [Component],
		components: {
			Value,
		},
		props: {
			apiGet: { mandatory: true, type: Function },
			pathList: { mandatory: true, type: Array },
			timestamp: { mandatory: true, type: Number },
		},
		data: function () {
			return {
				historyValue: [],
				historyContinuation: null,
				historyLoadingMore: false,
				pageSize: 20,
				timeout: null,
				isDestroyed: false,
			};
		},
		watch: {
			pathList() {
				this.reset();
			},
		},
		mounted() {
			this.fetch();
		},
		beforeUnmount() {
			this.isDestroyed = true;
			clearTimeout(this.timeout);
		},
		methods: {
			reset() {
				this.historyValue = [];
				this.historyContinuation = null;
				this.historyLoadingMore = false;
				clearTimeout(this.timeout);
				this.fetch();
			},
			async fetch() {
				await this.handleSubmit(async () => {
					const newest = this.historyValue.length ? this.historyValue[0][0] : null;
					const query =
						newest === null
							? { metadata: 1, count: this.pageSize }
							: { metadata: 1, count: this.pageSize, after: newest };
					const value = await this.apiGet(query);
					if (newest === null) {
						this.historyValue = value.data ?? [];
						this.historyContinuation = value.continuation ?? null;
					} else if (value.data?.length) {
						this.historyValue = [...value.data, ...this.historyValue];
					}
				});
				if (!this.isDestroyed) {
					this.timeout = setTimeout(this.fetch, 1000);
				}
			},
			async loadMoreHistory() {
				if (this.historyLoadingMore || !this.historyContinuation) {
					return;
				}
				this.historyLoadingMore = true;
				try {
					await this.handleSubmit(async () => {
						const value = await this.apiGet({
							metadata: 1,
							count: this.pageSize,
							continuation: JSON.stringify(this.historyContinuation),
						});
						this.historyValue = this.historyValue.concat(value.data ?? []);
						this.historyContinuation = value.continuation ?? null;
					});
				} finally {
					this.historyLoadingMore = false;
				}
			},
			onScroll() {
				const el = this.$refs.scroll;
				if (el.scrollTop + el.clientHeight >= el.scrollHeight - 40) {
					this.loadMoreHistory();
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	.history-scroll {
		max-height: 300px;
		overflow-y: auto;
		padding: 20px 0;
	}
</style>
