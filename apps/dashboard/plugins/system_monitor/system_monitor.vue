<template>
	<div class="system-monitor">
		{{ metadata }}
	</div>
</template>

<script>
	export default {
		props: {
			metadata: { type: Object, mandatory: true },
		},
		data: function () {
			return {};
		},
		computed: {},
		methods: {},
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
        $bzdIconNames: play pause stop next previous
    );

	.system-monitor {
	}
</style>
