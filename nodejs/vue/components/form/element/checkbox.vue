<template>
	<span :class="containerClass" @click="handleClick">
		<input
			type="checkbox"
			:id="uid"
			:checked="get()"
			:tabindex="disable ? undefined : 0"
			:disabled="disable"
			@focus="setActive"
			@blur="setInactive"
		/>
		<label :for="'id-checkbox-' + uid"><slot v-if="text.length == 0"></slot>{{ text }}</label>
	</span>
</template>

<script>
	import Element from "./element.vue";

	export default {
		mixins: [Element],
		computed: {
			/// ---- CONFIG ----------------------------------------
			text() {
				return this.getOption("text", "");
			},
			/// ---- IMPLEMENTATION ----------------------------------
			valueType() {
				return "boolean";
			},
			containerClass() {
				return {
					"irform-checkbox": true,
					[this.getOption("class")]: true,
				};
			},
		},
		methods: {
			handleClick() {
				this.$el.firstChild.focus();
				this.set(!this.get(), { action: "update" });
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/checkbox.scss" as *;
</style>
