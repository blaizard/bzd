<template>
	<div class="terminal" ref="terminal">
		<template v-for="line in reverse(content)">
			<div>
				<span v-for="data in groupByStyleId(line)" :style="styles[data[1]]">{{ data[0] }}</span>
			</div>
		</template>
	</div>
</template>

<script>
	import ExceptionFactory from "bzd/core/exception.mjs";

	const Exception = ExceptionFactory("terminal");
	const colors8 = ["#000000", "#cc0000", "#4e9a06", "#c4a000", "#729fcf", "#75507b", "#06989a", "#d3d7cf"];
	const colorsBright8 = ["#555753", "#ef2929", "#8ae234", "#fce94f", "#32afff", "#ad7fa8", "#34e2e2", "#ffffff"];

	// Spec: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
	export default {
		data: function () {
			return {
				content: [],
				styles: [],
				stylesMap: {},
				styleId: -1,
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
			/// Group characters by their style identifiers.
			groupByStyleId(line) {
				let styleId = -1;
				let output = [];
				for (const data of line) {
					if (data[1] != styleId) {
						styleId = data[1];
						output.push(["", styleId]);
					}
					output.at(-1)[0] += data[0];
				}
				return output;
			},
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
						this.content[this.y].push([" ", 0]);
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
							this.$set(this.content[this.y], this.x, [c, this.styleId]);
							++this.x;
						}
					}
				} while (i < size);
			},
			moveCursorUp(lines) {
				Exception.assert(typeof lines == "number", "Must be a number.");
				this.y = Math.max(this.y - lines, 0);
				return true;
			},
			moveCursorDown(lines) {
				Exception.assert(typeof lines == "number", "Must be a number.");
				this.y += lines;
				return true;
			},
			moveCursorRight(columns) {
				Exception.assert(typeof columns == "number", "Must be a number.");
				this.x += columns;
				return true;
			},
			moveCursorLeft(columns) {
				Exception.assert(typeof columns == "number", "Must be a number.");
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
				Exception.assert(typeof column == "number", "Must be a number.");
				this.x = column;
				return true;
			},
			moveCursorLineColumn(line = 0, column = 0) {
				Exception.assert(typeof line == "number", "Must be a number.");
				Exception.assert(typeof column == "number", "Must be a number.");
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
						this.$set(this.content[this.y], x, [" ", 0]);
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
			eraseScreen(arg) {
				// Erase from cursor until end of screen
				if (!arg) {
					this.eraseLine(0);
					this.content.splice(this.y);
				}
				// Erase from cursor to beginning of screen
				else if (arg == 1) {
					this.eraseLine(1);
					this.content.splice(0, this.y);
				}
				// Erase entire screen
				else if (arg == 2) {
					this.content = [];
				}
				// Erase saved lines
				else if (arg == 3) {
					// Ignore.
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
				if (!(style in this.stylesMap)) {
					this.styles.push(style);
					this.stylesMap[style] = this.styles.length - 1;
				}
				this.styleId = this.stylesMap[style];
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
					f: this.moveCursorLineColumn,
					G: this.moveCursorColumn,
					H: this.moveCursorLineColumn,
					J: this.eraseScreen,
					K: this.eraseLine,
				};

				let start = 0;
				const replacer = (match, params, name, offset) => {
					const args = params
						.split(";")
						.filter(Boolean)
						.map((i) => parseInt(i));

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
			*reverse(content) {
				for (let i = content.length - 1; i >= 0; --i) {
					yield content[i];
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	.terminal {
		width: 100%;
		height: 100%;
		padding: 10px;
		background-color: #222;
		color: #ddd;
		overflow: auto;
		display: flex;
		flex-direction: column-reverse;

		> * {
			white-space: pre-wrap;
		}
	}
</style>
