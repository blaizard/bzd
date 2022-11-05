<template>
	<div class="terminal">
		<template v-for="row in content">
			<span v-for="c in row" :style="styles[c[1]]">{{ c[0] }}</span><br />
		</template>
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
				x: 0,
				y: 0,
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
					else if (arg == 39) {
						this.terminalStyle.colorText = "inherit";
					}
					else if (arg >= 40 && arg <= 47) {
						this.terminalStyle.colorBackground = colors8[arg - 40];
					}
					else if (arg == 49) {
						this.terminalStyle.colorText = "transparent";
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
				const size = content.length;
				let i = 0;
				do {
					// If the row does not exists, create it. The loop need to cover this.y, hence the <= sign.
					for (let y = this.content.length; y <= this.y; ++y) {
						this.$set(this.content, y, []);
					}
					// If the column does not exists, create it. The loop does not need to cover this.x because this will be done when writing.
					for (let x = this.content[this.y].length; x < this.x; ++x) {
						this.content[this.y].push([" ", this.currentStyle]);
					}
					// Fill the content.
					while (i < size) {
						const c = content[i++];
						if (c == "\n") {
							++this.y;
							this.x = 0;
							break;
						}
						else if (c == "\r") {
							this.x = 0;
							break;
						}
						else if (c == "\b") {
							this.x = Math.max(this.x - 1, 0);
						}
						else {
							this.$set(this.content[this.y], this.x, [c, this.currentStyle]);
							++this.x;
						}
					}
				} while (i < size);
			},
			moveCursorUp(lines) {
				this.y = Math.max(this.y - lines, 0);
				return true;
			},
			moveCursorDown(lines) {
				this.y += lines;
				return true;
			},
			moveCursorRight(columns) {
				this.x += columns;
				return true;
			},
			moveCursorLeft(columns) {
				this.x = Math.max(this.x - columns, 0);
				return true;
			},
			moveCursorDownBegining(lines) {
				this.moveCursorDown(lines);
				this.x = 0;
				return true;
			},
			moveCursorUpBegining(lines) {
				this.moveCursorUp(lines);
				this.x = 0;
				return true;
			},
			moveCursorColumn(column) {
				this.x = column;
				return true;
			},
			moveCursorLineColumn(line = 0, column = 0) {
				this.x = column;
				this.y = line;
				return true;
			},
			eraseLine(arg) {
				// Erase from cursor to end of line
				if (!arg) {
					this.content[this.y] = this.content[this.y].slice(0, this.x);
				}
				// Erase start of line to the cursor
				else if (arg == 1) {
					for (let x = 0; x < this.x; ++x) {
						this.$set(this.content[this.y], x, [" ", this.currentStyle]);
					}
				}
				// Erase the entire line
				else if (arg == 2) {
					this.$set(this.content, this.y, []);
				}
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
					B: this.moveCursorDown,
					C: this.moveCursorRight,
					D: this.moveCursorLeft,
					E: this.moveCursorDownBegining,
					F: this.moveCursorUpBegining,
					G: this.moveCursorColumn,
					H: this.moveCursorLineColumn,
					f: this.moveCursorLineColumn,
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
