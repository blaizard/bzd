<template>
	<div>
		<div class="coverage">{{ (metadata.coverage * 100).toFixed(1) }}<small>%</small></div>
		<div class="metrics">{{ metadata.files }} <small>files</small> / {{ metadata.lines }} <small>lines</small></div>
	</div>
</template>

<script>
	export default {
		props: {
			metadata: { type: Object, mandatory: true },
			description: { type: Object, mandatory: true },
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
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
        $bzdIconNames: link
    );
</style>

<style lang="scss" scoped>
	.coverage {
		height: 150px;
		line-height: 150px;
		text-align: center;
		font-size: 48px;
	}
	.metrics {
		text-align: center;
		height: 100px;
		line-height: 100px;
		font-size: 20px;
	}
</style>
