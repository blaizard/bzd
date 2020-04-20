<template>
    <div class="bzd-menu-entry">
        <div class="bzd-menu-entry-wrapper" v-tooltip="tooltipObject" @click="handleClick">
            <i v-if="icon" :class="iconClass"></i>
            <span class="bzd-menu-entry-text" v-text="text"></span>
        </div>
        <div class="bzd-menu-nested">
            <slot></slot>
        </div>
    </div>
</template>

<script>
    "use strict"

	import DirectiveTooltip from "[bzd]/vue/directives/tooltip.js"

    export default {
        props: {
			text: { type: String, required: true },
			icon: { type: String, required: false, default: null },
			tooltip: { type: Object, required: false, default: null },
        },
		directives: {
			"tooltip": DirectiveTooltip
		},
        computed: {
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
            }
        }
    };
</script>
