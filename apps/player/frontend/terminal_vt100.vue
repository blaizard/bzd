<template>
	<div class="terminal-vt100" ref="terminal">
		<template v-for="line in reverse(content)">
			<div>
				<span v-for="data in groupByStyleId(line)" :style="styles[data[1]]">{{ data[0] }}</span>
			</div>
		</template>
	</div>
</template>

<script>
	import ExceptionFactory from "bzd/core/exception.mjs";
	import LogFactory from "bzd/core/log.mjs";

	const Exception = ExceptionFactory("terminal-vt100");
	const Log = LogFactory("terminal-vt100");
	const colors8 = ["#000000", "#cc0000", "#4e9a06", "#c4a000", "#729fcf", "#75507b", "#06989a", "#d3d7cf"];
	const colorsBright8 = ["#555753", "#ef2929", "#8ae234", "#fce94f", "#32afff", "#ad7fa8", "#34e2e2", "#ffffff"];

	// Spec: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
	// https://chromium.googlesource.com/apps/libapps/+/a5fb83c190aa9d74f4a9bca233dac6be2664e9e9/hterm/doc/ControlSequences.md#OSC
	export default {
		data: function () {
			return {
				content: [],
				styles: [],
				stylesMap: {},
				styleId: -1,
				x: 0,
				y: 0,
				remainder: "",
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
					const update = this.stream.reduce((obj, value) => obj + value, this.remainder);
					this.remainder = this.process(update);
					if (count) {
						this.$emit("processed", count);
					}
				},
				immediate: true,
			},
		},
		computed: {
			categoryMapping() {
				const csiFctMapping = {
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
				const csiMapping = [
					new RegExp("([0-9;]*)([^0-9;])", "g"),
					(_, params, name) => {
						const args = params
							.split(";")
							.filter(Boolean)
							.map((i) => parseInt(i));
						return (csiFctMapping[name] || (() => false))(...args);
					},
				];
				const oscFctMapping = {
					0: this.ignore("Set title"),
					2: this.ignore("Set title"),
					4: this.ignore("Set/read color palette"),
					9: this.ignore("iTerm2 Growl notifications"),
					10: this.ignore("Set foreground color"),
					11: this.ignore("Set background color"),
					50: this.ignore("Set the cursor shape"),
					52: this.ignore("Clipboard operations"),
					777: this.ignore("rxvt-unicode (urxvt) modules"),
				};
				const oscMapping = [
					new RegExp("([0-9]+);(.*?)\x07", "g"),
					(_, name, params) => {
						return (oscFctMapping[name] || (() => false))(params);
					},
				];
				return {
					"\x1b[": csiMapping,
					"\x9b": csiMapping,
					"\x1b]": oscMapping,
				};
			},
			regexprCategory() {
				const string = Object.keys(this.categoryMapping).map(this.escapeRegexpr).join("|");
				return new RegExp("(" + string + ")", "g");
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
					display: "inline",
				};
			},
			ignore(name) {
				return (...args) => {
					Log.warning("Ignoring command '{}' with args: {}.", name, [...args].toString());
					return true;
				};
			},
			sgr(...args) {
				for (const arg of args) {
					if (arg == 0) {
						this.resetTerminalStyle();
					} else if (arg == 1) {
						this.terminalStyle.fontWeight = "bold";
					} else if (arg == 3) {
						this.terminalStyle.fontStyle = "italic";
					} else if (arg == 4) {
						this.terminalStyle.textDecoration = "underline";
					} else if (arg == 8) {
						this.terminalStyle.display = "none";
					} else if (arg == 9) {
						this.terminalStyle.textDecoration = "line-through";
					} else if (arg == 22) {
						this.terminalStyle.fontWeight = "normal";
					} else if (arg == 23) {
						this.terminalStyle.fontStyle = "normal";
					} else if (arg == 24) {
						this.terminalStyle.textDecoration = "none";
					} else if (arg == 28) {
						this.terminalStyle.display = "inline";
					} else if (arg == 29) {
						this.terminalStyle.textDecoration = "none";
					} else if (arg >= 30 && arg <= 37) {
						this.terminalStyle.colorText = colors8[arg - 30];
					} else if (arg == 39) {
						this.terminalStyle.colorText = "inherit";
					} else if (arg >= 40 && arg <= 47) {
						this.terminalStyle.colorBackground = colors8[arg - 40];
					} else if (arg == 49) {
						this.terminalStyle.colorText = "transparent";
					} else if (arg >= 90 && arg <= 97) {
						this.terminalStyle.colorText = colorsBright8[arg - 90];
					} else if (arg >= 100 && arg <= 107) {
						this.terminalStyle.colorBackground = colorsBright8[arg - 100];
					} else {
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
						} else if (c == "\r") {
							this.x = 0;
							break;
						} else if (c == "\b") {
							this.x = Math.max(this.x - 1, 0);
						} else if (c == "\x07") {
							// Bell character - ignore
						} else {
							this.$set(this.content[this.y], this.x, [c, this.styleId]);
							++this.x;
						}
					}
				} while (i < size);
			},
			moveCursorUp(lines = 0) {
				Exception.assert(typeof lines == "number", "Must be a number, instead: {}", lines);
				this.y = Math.max(this.y - lines, 0);
				return true;
			},
			moveCursorDown(lines) {
				Exception.assert(typeof lines == "number", "Must be a number, instead: {}", lines);
				this.y += lines;
				return true;
			},
			moveCursorRight(columns) {
				Exception.assert(typeof columns == "number", "Must be a number, instead: {}", columns);
				this.x += columns;
				return true;
			},
			moveCursorLeft(columns) {
				Exception.assert(typeof columns == "number", "Must be a number, instead: {}", columns);
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
				} else {
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
				} else {
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
			escapeRegexpr(string) {
				return string.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
			},
			cloneRegexpr(regexpr, position = 0) {
				regexpr.lastIndex = position;
				return regexpr;
			},
			process(stream) {
				let regexprCategory = this.cloneRegexpr(this.regexprCategory);
				let match;
				let position = 0;
				while ((match = regexprCategory.exec(stream)) !== null) {
					// Write everything that did not match.
					const matchStr = match[0];
					const offset = match.index;
					this.write(stream.substring(position, offset));
					position = offset + matchStr.length;

					// Associate to its match.
					Exception.assert(matchStr in this.categoryMapping, "The regular expression must match something.");
					let [regexpr, handler] = this.categoryMapping[matchStr];

					regexpr.lastIndex = position;
					if (!(match = regexpr.exec(stream))) {
						// Stream unfinished.
						return stream.substring(offset);
					}
					// No matches right after the category.
					if (match.index !== position) {
						Log.error(
							"Command '{}' followed with '{}...' is malformed or not supported.",
							matchStr,
							stream.substring(0, 10)
						);
						continue;
					}
					// Process the function.
					if (!handler(...match)) {
						Log.error("Command '{}{}' is malformed or not supported.", matchStr, match[0]);
					}

					position = regexpr.lastIndex;
					regexprCategory.lastIndex = position;
				}
				this.write(stream.substring(position));
				return "";
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
	.terminal-vt100 {
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
			word-break: break-all;
		}
	}
</style>
