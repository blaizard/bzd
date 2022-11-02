<template>
	<div class="terminal">
		<span v-for="line in lines" v-html="fromANSItoHTML(line)"></span>
	</div>
</template>

<script>
	const colors8 = ["#000000", "#cc0000", "#4e9a06", "#c4a000", "#729fcf", "#75507b", "#06989a", "#d3d7cf"];
	const colorsBright8 = ["#555753", "#ef2929", "#8ae234", "#fce94f", "#32afff", "#ad7fa8", "#34e2e2", "#ffffff"];

	export default {
		data: function () {
			return {
				fontWeight: "normal",
				fontStyle: "normal",
				textDecoration: "none",
				colorText: "initial",
				colorBackground: "transparent",
			};
		},
		props: {
			content: { type: Array, mandatory: false, default: () => [] },
		},
		computed: {
			lines() {
				return this.content
					.reduce((obj, value) => {
						obj.push(...value.split("\n"));
						return obj;
					}, [])
					.reverse();
				//return [...this.content].reverse();
			},
		},
		methods: {
			sgr(...args) {
				for (const arg of args) {
					if (arg == 0) {
						this.fontWeight = "normal";
						this.fontStyle = "normal";
						this.textDecoration = "none";
						this.colorText = "initial";
						this.colorBackground = "transparent";
					}
					else if (arg == 1) {
						this.fontWeight = "bold";
					}
					else if (arg == 3) {
						this.fontStyle = "italic";
					}
					else if (arg == 4) {
						this.textDecoration = "underline";
					}
					else if (arg >= 30 && arg <= 37) {
						this.colorText = colors8[arg - 30];
					}
					else if (arg >= 40 && arg <= 47) {
						this.colorBackground = colors8[arg - 40];
					}
					else if (arg >= 90 && arg <= 97) {
						this.colorText = colorsBright8[arg - 90];
					}
					else if (arg >= 100 && arg <= 107) {
						this.colorBackground = colorsBright8[arg - 100];
					}
				}
				return this.updateStyle();
			},
			updateStyle(isFirst = false) {
				return (
					(isFirst ? "" : "</span>") +
					"<span style=\"color: " +
					this.colorText +
					"; background-color: " +
					this.colorBackground +
					"; font-weight: " +
					this.fontWeight +
					"; font-style: " +
					this.fontStyle +
					"; text-decoration: " +
					this.textDecoration +
					";\">"
				);
			},
			fromANSItoHTML(line) {
				const fctMapping = {
					m: this.sgr,
				};
				const replacer = (match, params, name) => {
					const args = params.split(";").map((i) => parseInt(i));
					console.log("<" + match + ">", args, "name=", name);
					return (
						fctMapping[name] ||
						(() => {
							console.error("Unknown function '" + name + "'");
							return "";
						})
					)(...args);
				};
				const regex = new RegExp("\\x1b\\[([0-9;]+)([a-zA-Z])", "g");
				return this.updateStyle(true) + line.replace(regex, replacer) + "</span>";
			},
		},
	};
</script>

<style lang="scss" scoped>
	.terminal {
		display: flex;
		flex-direction: column-reverse;
		width: 100%;

		> * {
			white-space: pre-wrap;
		}
	}
</style>
