<template>
	<TerminalVt100 :stream="stream" @processed="handleStreamProcessed" tabindex="-1"></TerminalVt100>
</template>

<script>
	import TerminalVt100 from "#bzd/nodejs/vue/components/terminal/frontend/vt100.vue";

	export default {
		components: {
			TerminalVt100,
		},
		data: function () {
			return {
				stream: [],
				websocket: null,
			};
		},
		props: {
			websocketEndpoint: { type: String, required: true },
		},
		async mounted() {
			this.websocket = await this.$websocket.handle(this.websocketEndpoint, (data) => {
				this.stream.push(data);
			});
			this.websocket.send(JSON.stringify({ type: "init", value: {} }));
			this.$el.addEventListener("keydown", this.handleKeyDown);
		},
		beforeUnmount() {
			this.$el.removeEventListener("keydown", this.handleKeyDown);
			this.websocket.close();
		},
		methods: {
			handleStreamProcessed(count) {
				this.stream.splice(0, count);
			},
			async setInput(value) {
				if (this.websocket) {
					this.websocket.send(JSON.stringify({ type: "stream", value: value }));
				}
			},
			async handleKeyDown(event) {
				const translationMap = {
					enter: "\n",
					backspace: "\b",
					escape: true,
					alt: true,
					control: true,
					wakeup: true,
					shift: true,
					capslock: true,
					tab: true,
					arrowup: true,
					arrowdown: true,
					arrowleft: true,
					arrowright: true,
				};
				const key = translationMap[event.key.toLowerCase()] || event.key;
				if (key !== true) {
					await this.setInput(key);
				}
			},
		},
	};
</script>
