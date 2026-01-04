<template>
	<TerminalVt100
		:stream="stream"
		@processed="handleStreamProcessed"
		@focus="hasFocus = true"
		@blur="hasFocus = false"
		@keydown="handleKeyDown"
		@paste="handlePaste"
		tabindex="-1"
		:readonly="readonly || !hasFocus"
	></TerminalVt100>
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
				hasFocus: false,
			};
		},
		props: {
			websocketEndpoint: { type: String, required: true },
			readonly: { type: Boolean, required: false, default: false },
		},
		async mounted() {
			this.websocket = await this.$websocket.handle(this.websocketEndpoint, (data) => {
				this.stream.push(data);
			});
			this.websocket.send(JSON.stringify({ type: "init", value: {} }));
		},
		beforeUnmount() {
			this.websocket.close();
		},
		methods: {
			handleStreamProcessed(count) {
				this.stream.splice(0, count);
			},
			async setInput(value) {
				if (this.websocket && !this.readonly) {
					this.websocket.send(JSON.stringify({ type: "stream", value: value }));
				}
			},
			async handleKeyDown(event) {
				// Ignore ctrl+<something>, this ensures that paste events for example are not handled by this.
				if (event.ctrlKey || event.metaKey) {
					return;
				}
				const translationMap = {
					enter: "\n",
					backspace: "\b",
					escape: true,
					alt: true,
					control: true,
					wakeup: true,
					shift: true,
					capslock: true,
					tab: "\t",
					arrowup: true,
					arrowdown: true,
					arrowleft: true,
					arrowright: true,
				};
				const key = translationMap[event.key.toLowerCase()] || event.key;
				if (key !== true) {
					event.preventDefault();
					await this.setInput(key);
				}
			},
			async handlePaste(event) {
				event.preventDefault();
				const text = (event.clipboardData || window.clipboardData).getData("text");
				await this.setInput("\x1b[200~" + text + "\x1b[201~");
			},
		},
	};
</script>
