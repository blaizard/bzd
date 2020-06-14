<template>
    <div class="bzd-menu-entry-wrapper">
        <div class="bzd-menu-entry" v-tooltip="tooltipObject" @click="handleClick">
            <i v-if="isIconClass" :class="iconClass"></i>
            <component v-else-if="isIconComponent" class="bzd-menu-entry-icon" :is="icon"></component>
            <span class="bzd-menu-entry-text" v-text="text"></span>
        </div>
        <div class="bzd-menu-nested">
            <slot></slot>
        </div>
    </div>
</template>

<script>


import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";

export default {
	props: {
		text: { type: String, required: true },
		icon: { type: String | Object, required: false, default: null },
		tooltip: { type: Object, required: false, default: null },
		link: { type: String, required: false, default: null },
	},
	directives: {
		"tooltip": DirectiveTooltip
	},
	computed: {
		isIconComponent() {
			return (this.icon && typeof this.icon == "object");
		},
		isIconClass() {
			return (this.icon && typeof this.icon == "string");
		},
		iconClass() {
			// Note it is important to start with the icon name
			return this.icon + " bzd-menu-entry-icon";
		},
		tooltipObject() {
			return (this.tooltip) ? this.tooltip : { "text": this.text };
		}
	},
	methods: {
		handleClick() {
			this.$emit("click");
			if (this.link) {
				this.$routerDispatch(this.link);
			}
		}
	}
};
</script>
