<template>
	<span class="irform-checkbox" @click="handleClick">
		<input type="checkbox"
				:id="uid"
				:checked="get()"
				:tabindex="(disable) ? undefined : 0"
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
	props: {
		value: {type: Boolean, required: false, default: false}
	},
	computed: {
		text() {
			return this.getOption("text", "");
		}
	},
	methods: {
		handleClick() {
			this.$el.firstChild.focus();
			this.set(!this.get());
		}
	}
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/checkbox.scss";
</style>
