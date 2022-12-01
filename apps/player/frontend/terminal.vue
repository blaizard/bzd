<template>
	<TerminalVt100 :stream="stream" @processed="handleStreamProcessed" tabindex="-1"></TerminalVt100>
</template>

<script>
	import TerminalVt100 from "./terminal_vt100.vue";

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
		async mounted() {
			this.websocket = await this.create();
			this.$el.addEventListener("keydown", this.handleKeyDown);
		},
		beforeUnmount() {
			this.$el.removeEventListener("keydown", this.handleKeyDown);
		},
		methods: {
			async create() {
				return new Promise((resolve, reject) => {
					let socket = new WebSocket("ws://localhost:9999");
					socket.onopen = () => {
						socket.send(JSON.stringify({ type: "init", value: {} }));
						resolve(socket);
					};
					socket.onmessage = (event) => {
						this.stream.push(event.data);
					};
					socket.onerror = (error) => {
						reject(error);
					};
				});
			},
			handleStreamProcessed(count) {
				this.stream.splice(0, count);
			},
			sleep(ms) {
				return new Promise((resolve) => setTimeout(resolve, ms));
			},
			async setInput(value) {
				while (!this.websocket) {
					await this.sleep(100);
				}
				this.websocket.send(JSON.stringify({ type: "stream", value: value }));
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
