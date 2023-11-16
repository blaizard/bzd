<template>
	<div class="bzd-menu-entry-wrapper" v-if="isAuthorized">
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
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		props: {
			text: { type: String, required: true },
			icon: { type: String | Object, required: false, default: null },
			tooltip: { type: Object, required: false, default: null },
			link: { type: String | Object, required: false, default: null },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		computed: {
			isIconComponent() {
				return this.icon && typeof this.icon == "object";
			},
			isIconClass() {
				return this.icon && typeof this.icon == "string";
			},
			iconClass() {
				// Note it is important to start with the icon name
				return this.icon + " bzd-menu-entry-icon";
			},
			tooltipObject() {
				return this.tooltip ? this.tooltip : { data: this.text };
			},
			authentication() {
				return typeof this.link == "object" ? this.link.authentication || false : false;
			},
			path() {
				return typeof this.link == "object" ? this.link.path : this.link;
			},
			isAuthorized() {
				if (!this.authentication) {
					return true;
				}
				// After this, authentication is required.
				if (typeof this.authentication == "boolean") {
					return this.$authentication.isAuthenticated;
				}
				if (typeof this.authentication == "string") {
					return this.$authentication.hasScope(this.authentication);
				}
				return false;
			},
		},
		methods: {
			handleClick() {
				this.$emit("click");
				if (this.path) {
					this.$routerDispatch(this.path);
				}
			},
		},
	};
</script>
