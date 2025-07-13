<template>
	<div>
		<h1>Job {{ job }}</h1>
		<TerminalWebsocket :websocket-url="websocketUrl" style="height: 600px"></TerminalWebsocket>
	</div>
</template>

<script>
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import TerminalWebsocket from "#bzd/nodejs/vue/components/terminal/frontend/websocket.vue";

	const Exception = ExceptionFactory("main");

	export default {
		mixins: [Component],
		components: {
			TerminalWebsocket,
		},
		props: {
			job: { type: String, required: true },
		},
		computed: {
			websocketUrl() {
				return "/api/v1/socket/job/" + this.job;
			},
		},
		data: function () {
			return {
				value: {},
			};
		},
	};
</script>

<style lang="scss"></style>
