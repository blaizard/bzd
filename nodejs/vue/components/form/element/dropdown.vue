<template>
	<DropdownTemplate
		:value="get()"
		@input="set"
		:disable="disable"
		:description="descriptionDropdown"
		@directInput="directValue = $event"
		@key="handleKey"
	>
		<div
			v-for="item in displayList"
			:class="{ 'irform-dropdown-item': true, 'irform-dropdown-item-selected': isSelected(item) }"
			v-html="formatMenuEntry(item.display)"
			@mousedown="select(item)"
		></div>
	</DropdownTemplate>
</template>

<script>
	import Element from "./element.vue";
	import DropdownTemplate from "./dropdown_template.vue";

	export default {
		mixins: [Element],
		components: {
			DropdownTemplate,
		},
		props: {
			value: { type: String | Array, required: false, default: "" },
		},
		data: function () {
			return {
				multi: this.getOption("multi", false),
				/**
				 * If the displayed values should be interpreted as HTML or not
				 */
				html: this.getOption("html", false),
				/**
				 * Filter the list when editing
				 */
				filter: this.getOption("filter", !this.isFctWithArg(this.getOption("list"))),
				/**
				 * Minimum weight for word matching
				 */
				minMatchWeight: this.getOption("minMatchWeight", 0.6),
				/**
				 * Maximum number of entries to display. If set to 0, there is no limit.
				 */
				max: this.getOption("max", 0),
				/*
				 * ---- Internal values ---------------------------------------
				 * The list of values as they should be printed
				 */
				list: [],
				filteredList: [],
				directValue: "",
			};
		},
		computed: {
			descriptionDropdown() {
				return Object.assign({}, this.description, {
					editable: this.getOption("editable", false),
					format: this.presets,
					process: this.process,
				});
			},
			presets() {
				return this.list.reduce((acc, item) => {
					acc[item.value] = item.display;
					return acc;
				}, {});
			},
			curValue() {
				if (this.multi) {
					return this.value instanceof Array ? this.value : [];
				}
				return String(this.get() || "");
			},
			rawList() {
				return this.getOption("list", []);
			},
			displayList() {
				return this.max ? this.filteredList.slice(0, this.max) : this.filteredList;
			},
			isListFct() {
				return typeof this.rawList === "function";
			},
			isListFctWithArg() {
				return this.isFctWithArg(this.rawList);
			},
		},
		watch: {
			rawList: {
				handler: async function () {
					this.list = [];
					await this.process(this.directValue);
				},
			},
		},
		methods: {
			formatMenuEntry(value) {
				return this.html ? value : this.toHtmlEntities(value);
			},

			isFctWithArg(arg) {
				return typeof arg === "function" && arg.length > 0;
			},

			async process(value) {
				if (this.list.length == 0 || this.isListFctWithArg) {
					this.list = await this.fetchList(value);
				}
				this.filteredList = this.filter ? this.processFilter(value) : this.list;
			},

			/**
			 * Filter the current list to generate the display list
			 */
			processFilter(text) {
				let list = this.list;

				if (text) {
					const value = text.toLowerCase();
					list = list
						.map((item) => {
							item.weight = 0;
							item.search.map((search) => {
								item.weight = Math.max(item.weight, this.getMatchingWeight(value, search, this.minMatchWeight));
							});
							return item;
						})
						.filter((item) => item.weight);
					list.sort((a, b) => b.weight - a.weight);
				}
				return list;
			},

			handleKey(keyCode) {
				// Arrow up
				if (keyCode == 38) {
					this.selectByOffset(-1);
				}
				// Arrow down
				else if (keyCode == 40) {
					this.selectByOffset(1);
				}
			},

			async fetchList(value) {
				try {
					const list = this.isListFct ? await this.rawList(value) : this.rawList;
					return this.createMultiValueList(list, this.html);
				}
			catch (e) {
					console.error("Error while fetching data: " + e);
				}
				return [];
			},

			/**
			 * Calculate the matching weight of a word in a sentence.
			 * It performs a fuzzy search on the word using a fast algorithm.
			 */
			getMatchingWeight(word, sentence, minMatch) {
				const firstChar = word.charAt(0);
				if (!firstChar) {
					return 0;
				}

				let start = -1;
				let weight = 0;
				while ((start = sentence.indexOf(firstChar, start + 1)) != -1) {
					// If this is the first character of a word, add a weight to prioritize this word
					let curWeight = !start ? 1 : 0.9;
					let iSentence = start;
					// Check if the word is +/- 1 character far away
					for (let i = 1; i < word.length; ++i) {
						const c = word.charAt(i);
						if (sentence.charAt(iSentence + 1) == c) {
							curWeight += 1;
							iSentence += 1;
						}
					else if (sentence.charAt(iSentence + 2) == c) {
							curWeight += 0.5;
							iSentence += 2;
						}
					else if (sentence.charAt(iSentence - 1) == c) {
							curWeight += 0.5;
						}
					else {
							++iSentence;
						}
					}

					weight = Math.max(weight, curWeight);
				}

				// Normalize the result and make the result exponential to add importance on full matches
				weight /= word.length;

				// Add a penalty on word size to ensure that an exact match alwasy get more weight than a substring match
				weight = word.length == sentence.length ? weight : weight * 0.9;

				return !minMatch || weight > minMatch ? weight : 0;
			},

			selectByOffset(offset) {
				// Capture the current selection if any
				let index = -1;
				for (const i in this.displayList) {
					if (this.isSelected(this.displayList[i])) {
						index = parseInt(i);
						break;
					}
				}

				index += offset;
				if (index >= 0 && index < this.displayList.length) {
					this.select(this.displayList[index]);
				}
			},

			select(item) {
				if (!this.isSelected(item)) {
					/*
					 * Copy the value before it got altered by the input element losing its focus,
					 * and after all event have been processed, set the new value.
					 */
					setTimeout(() => {
						if (this.multi) {
							const curValue = this.curValue.splice(0);
							this.set(curValue.concat(String(item.value)));
						}
					else {
							this.set(String(item.value));
						}
					}, 10);
				}
			},

			isSelected(item) {
				if (this.multi) {
					return this.curValue.indexOf(item.value) != -1;
				}
				return this.curValue === item.value;
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd-style/css/form/dropdown.scss";
</style>
