<template>
	<div :class="containerClass">
		<ElementInput
			:class="{ 'irform-dropdown-select': true, active: isActive }"
			:description="inputDescription"
			:disable="disable"
			:value="curValue"
			@input="set"
			@directInput="handleDirectInput"
			@active="handleActive"
			@key="handleKey"
		>
		</ElementInput>
		<div :class="{ 'irform-dropdown-menu': true, 'irform-dropdown-menu-loading': loading }" ref="menu">
			<slot></slot>
		</div>
	</div>
</template>

<script>
import Element from "./element.vue";
import ElementInput from "./input.vue";

export default {
	mixins: [Element],
	components: {
		ElementInput,
	},
	props: {
		value: { type: String | Array, required: true },
	},
	data: function () {
		return {
			// Value that is typed in the control
			directValue: "",
			// Flag set when the item is loading
			loading: false,
			// Timeout when the processList is called
			processTimeout: null,
		};
	},
	computed: {
		containerClass() {
			return {
				"irform-dropdown": true,
				"irform-loading": this.loading,
				"irform-empty": !this.$slots.default,
				[this.getOption("class")]: true,
			};
		},
		delay() {
			return this.getOption("delay", 1);
		},
		curValue() {
			if (this.getOption("multi", false)) {
				return this.value instanceof Array ? this.value : [];
			}
			return String(this.get() || "");
		},
		inputDescription() {
			return Object.assign({}, this.description, {
				post: { html: "<span class=\"irform-dropdown-arrow\">&nbsp;</span>" },
			});
		},
		process() {
			return this.getOption("process", () => {});
		},
	},
	watch: {
		directValue: {
			immediate: true,
			handler: function (value) {
				// Kill any previous loading update
				if (this.processTimeout) {
					clearTimeout(this.processTimeout);
					this.loading = false;
				}
				// Process asynchronously to avoid any blocking
				if (!this.loading) {
					this.loading = true;
					this.processTimeout = setTimeout(() => {
						this.processInput(value);
					}, this.delay);
				}
			},
		},
	},
	methods: {
		async processInput(value) {
			try {
				this.processTimeout = null;
				await this.process(value);
			}
			finally {
				this.loading = false;
				// Set the list to the top
				if (this.$refs.menu) {
					this.$refs.menu.scrollTop = 0;
				}
			}
			// Process the final value if it changed
			if (value != this.directValue) {
				this.processInput(this.directValue);
			}
		},

		isFctWithArg(arg) {
			return typeof arg === "function" && arg.length > 0;
		},

		handleKey(keyCode) {
			// Escape
			if (keyCode == 27) {
				this.$el.getElementsByClassName("irform-input-body")[0].blur();
			}
			this.$emit("key", keyCode);
		},

		handleDirectInput(value) {
			this.directValue = value;
			this.$emit("directInput", value);
		},
	},
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/dropdown.scss";
</style>
