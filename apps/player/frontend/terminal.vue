<template>
	<div class="terminal">
		<span v-for="c in content" :style="styles[c[1]]">{{ c[0] }}</span>
	</div>
</template>

<script>
	const colors8 = ["#000000", "#cc0000", "#4e9a06", "#c4a000", "#729fcf", "#75507b", "#06989a", "#d3d7cf"];
	const colorsBright8 = ["#555753", "#ef2929", "#8ae234", "#fce94f", "#32afff", "#ad7fa8", "#34e2e2", "#ffffff"];

	// Spec: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
	export default {
		data: function () {
			return {
				content: [],
				styles: [],
				lines: [],
				current: "",
			};
		},
		created() {
			this.resetTerminalStyle();
			this.terminalCursor = 0;
			this.createStyle();
		},
		props: {
			stream: { type: Array, mandatory: false, default: () => [] },
		},
		computed: {
			currentStyle() {
				return this.styles.length - 1;
			},
		},
		watch: {
			stream: {
				handler() {
					const count = this.stream.length;
					const update = this.stream.reduce((obj, value) => obj + value, "");
					this.process(update);
					if (count) {
						this.$emit("processed", count);
					}
				},
				immediate: true,
			},
		},
		methods: {
			resetTerminalStyle() {
				this.terminalStyle = {
					fontWeight: "normal",
					fontStyle: "normal",
					textDecoration: "none",
					colorText: "inherit",
					colorBackground: "transparent",
				};
			},
			sgr(...args) {
				for (const arg of args) {
					if (arg == 0) {
						this.resetTerminalStyle();
					}
					else if (arg == 1) {
						this.terminalStyle.fontWeight = "bold";
					}
					else if (arg == 3) {
						this.terminalStyle.fontStyle = "italic";
					}
					else if (arg == 4) {
						this.terminalStyle.textDecoration = "underline";
					}
					else if (arg >= 30 && arg <= 37) {
						this.terminalStyle.colorText = colors8[arg - 30];
					}
					else if (arg >= 40 && arg <= 47) {
						this.terminalStyle.colorBackground = colors8[arg - 40];
					}
					else if (arg >= 90 && arg <= 97) {
						this.terminalStyle.colorText = colorsBright8[arg - 90];
					}
					else if (arg >= 100 && arg <= 107) {
						this.terminalStyle.colorBackground = colorsBright8[arg - 100];
					}
					else {
						return false;
					}
				}
				this.createStyle();
				return true;
			},
			// Write content to the stream at the cursor position.
			write(content) {
				for (const c of content) {
					this.content[this.terminalCursor++] = [c, this.currentStyle];
				}
			},
			moveCursorUp(lines) {
				const indexOfPreviousLine = (index) => {
					for (let i = index; i >= 0; --i) {
						if (this.content[i][0] == "\n") {
							return i;
						}
					}
					return 0;
				};

				let index = indexOfPreviousLine(this.terminalCursor - 1);
				const distance = this.terminalCursor - index - 1;
				while (lines--) {
					index = indexOfPreviousLine(index - 1);
					this.terminalCursor = index + distance;
				}
				return true;
			},
			eraseLine(arg) {
				// Erase from cursor to end of line
				if (!arg) {
					this.content = this.content.slice(0, this.terminalCursor);
				}
				// Erase start of line to the cursor
				/*else if (arg == 1) {
				}
				// Erase the entire line
				else if (arg == 2) {
				}*/
				else {
					return false;
				}
				return true;
			},
			createStyle() {
				const style =
					"color: " +
					this.terminalStyle.colorText +
					"; background-color: " +
					this.terminalStyle.colorBackground +
					"; font-weight: " +
					this.terminalStyle.fontWeight +
					"; font-style: " +
					this.terminalStyle.fontStyle +
					"; text-decoration: " +
					this.terminalStyle.textDecoration +
					";";
				this.styles.push(style);
			},
			process(stream) {
				const fctMapping = {
					m: this.sgr,
					A: this.moveCursorUp,
					K: this.eraseLine,
				};

				let start = 0;
				const replacer = (match, params, name, offset) => {
					const args = params.split(";").map((i) => parseInt(i));

					// Update the content.
					this.write(stream.substring(start, offset));
					start = offset + match.length;

					const result = (fctMapping[name] || (() => false))(...args);
					if (result !== true) {
						console.error("Unknown function '" + name + "' with params " + args);
					}

					return match;
				};

				const regex = new RegExp("\\x1b\\[([0-9;]*)([a-zA-Z])", "g");
				stream.replace(regex, replacer);
				this.write(stream.substring(start));
			},
		},
	};
</script>

<style lang="scss" scoped>
	.terminal {
		//display: flex;
		//flex-direction: column-reverse;
		width: 100%;
		background-color: #222;
		color: #ddd;

		> * {
			white-space: pre-wrap;
		}
	}
</style>
