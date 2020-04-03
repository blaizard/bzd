<template>
	<div :class="containerClass">
		<ElementInput :class="{'irform-dropdown-select': true, 'active': isActive}"
				:description="inputDescription"
				:disable="disable"
				:value="curValue"
				@input="set"
				@directInput="directValue = $event"
				@active="handleActive"
				@key="handleKey">
		</ElementInput>
		<div v-if="html" :class="{'irform-dropdown-menu': true, 'irform-dropdown-menu-loading': loading}" ref="menu">
			<div v-for="item in displayList" :class="{'irform-dropdown-item': true, 'irform-dropdown-item-selected': isSelected(item)}" v-html="item.display" @mousedown="select(item)"></div>
		</div>
		<div v-else :class="{'irform-dropdown-menu': true, 'irform-dropdown-menu-loading': loading}" ref="menu">
			<div v-for="item in displayList" :class="{'irform-dropdown-item': true, 'irform-dropdown-item-selected': isSelected(item)}" v-text="item.display" @mousedown="select(item)"></div>
		</div>
	</div>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	import ElementInput from "./input.vue"

	export default {
		mixins: [Element],
		components: {
			ElementInput
		},
		props: {
			value: {type: String | Array, required: false, default: ""}
		},
		data: function() {
			return {
				/**
				 * If the displayed values should be interpreted as HTML or not
				 */
				html: this.getOption("html", false),
				/**
				 * If the element should be editable or not
				 */
				editable: this.getOption("editable", false),
				/**
				 * This option will allow any value. If set to false, it will allow
				 * only the preset values.
				 */
				any: this.getOption("any", true),
				/**
				 * Placeholder if no value is input
				 */
				placeholder: this.getOption("placeholder", ""),
				/**
				 * If multiple values are allowed. If set, it will return an array.
				 */
				multi: this.getOption("multi", false),
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
				/**
				 * Delay used to fetch and show the updated list
				 */
				delay: this.getOption("delay", 1),
				// ---- Internal values ---------------------------------------
				post: {html: "<span class=\"irform-dropdown-arrow\">&nbsp;</span>"},
				// Value that is typed in the control
				directValue: "",
				// The list of values as they should be printed
				list: [],
				// List to be displayed, after the filter passed on it
				filteredList: [],
				// Flag set when the item is loading
				loading: false,
				// Timeout when the processList is called
				processTimeout: null
			}
		},
		computed: {
			containerClass() {
				return {
					"irform-dropdown": true,
					"irform-loading": this.loading
				};
			},
			presets() {
				return this.list.reduce((acc, item) => {
					acc[item.value] = item.display;
					return acc;
				}, {});
			},
			curValue() {
				if (this.multi) {
					return (this.value instanceof Array) ? this.value : [];
				}
				return String(this.get() || "");
			},
			displayList() {
				return (this.max) ? this.filteredList.slice(0, this.max) : this.filteredList;
			},
			isListFct() {
				return (typeof this.getOption("list", []) === "function");
			},
			isListFctWithArg() {
				return this.isFctWithArg(this.getOption("list"));
			},
			inputDescription() {
				return {
					editable: this.editable,
					html: this.html,
					presets: this.presets,
					multi: this.multi,
					placeholder: this.placeholder,
					any: this.any,
					post: this.post
				};
			}
		},
		watch: {
			directValue: {
				immediate: true,
				handler: function(value) {
					// Kill any previous loading update
					if (this.processTimeout) {
						clearTimeout(this.processTimeout);
						this.loading = false;
					}
					// Process asynchronously to avoid any blocking
					if (!this.loading) {
						this.loading = true;
						this.processTimeout = setTimeout(() => this.processList(value), this.delay);
					}
				}
			}
		},
		methods: {

			isFctWithArg(arg) {
				return (typeof arg === "function") && (arg.length > 0);
			},

			/**
			 * Entry point to process the list and filteredList
			 */
			async processList(text) {
				try {
					this.processTimeout = null
					await this.fetchList(text);
					this.filteredList = (this.filter) ? await this.processFilter(text) : this.list;
				}
				finally {
					this.loading = false;
					// Set the list to the to
					if (this.$refs.menu) {
						this.$refs.menu.scrollTop = 0;
					}
				}
				// Process the final value if it changed
				if (text != this.directValue) {
					this.processList(this.directValue);
				}
			},

			/**
			 * Filter the current list to generate the display list
			 */
			processFilter(text) {
				let list = this.list;

				if (text) {
					const value = text.toLowerCase();
					list = list.map((item) => {
						item.weight = 0;
						item.search.map((search) => {
							item.weight = Math.max(item.weight, this.getMatchingWeight(value, search, this.minMatchWeight));
						});
						return item;
					}).filter((item) => item.weight);
					list.sort((a, b) => (b.weight - a.weight));
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
				// Escape
				else if (keyCode == 27) {
					this.$el.getElementsByClassName("irform-input-body")[0].blur();
				}
			},

			async fetchList(value) {
				const listOrFct = this.getOption("list", []);

				// Fetch multilpe times if the function takes at least 1 argument
				if (this.list.length == 0 || this.isListFctWithArg) {
					try {
						const list = (this.isListFct) ? await listOrFct(value) : listOrFct;
						this.list = this.createMultiValueList(list, this.html);
					}
					catch (e) {
						console.error("Error while fetching data: " + e);
						this.list = [];
					}
				}
			},

			/**
			 * Calculate the matching weight of a word in a sentence.
			 * It performs a fuzzy search on the word using a fast algorithm.
			 */
			getMatchingWeight (word, sentence, minMatch) {

				const firstChar = word.charAt(0);
				if (!firstChar) return 0;

				let start = -1;
				let weight = 0;
				while ((start = sentence.indexOf(firstChar, start + 1)) != -1) {
					// If this is the first character of a word, add a weight to prioritize this word
					let curWeight = (!start) ? 1 : 0.9;
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
				weight = (word.length == sentence.length) ? weight : weight * 0.9;

				return (!minMatch || weight > minMatch) ? weight : 0;
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
				if (!this.loading) {
					if (!this.isSelected(item)) {
						// Copy the value before it got altered by the input element losing its focus,
						// and after all event have been processed, set the new value.
						setTimeout(() => {
							if (this.multi) {
								const curValue = this.curValue.splice(0);
								this.set(curValue.concat(item.value));
							}
							else {
								this.set(item.value);
							}
						}, 10);
					}
				}
			},

			isSelected(item) {
				if (this.multi) {
					return (this.curValue.indexOf(item.value) != -1);
				}
				return (this.curValue === item.value);
			}
		}
	}
</script>
