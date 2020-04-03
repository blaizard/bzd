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
		<label :for="'id-checkbox-' + uid">{{ getOption("text", "") }}</label>
	</span>
</template>

<script>
	"use strict";

	import Element from "./element.vue"

	export default {
		mixins: [Element],
		props: {
			value: {type: Boolean, required: false, default: false}
		},
		methods: {
			handleClick() {
				this.$el.firstChild.focus();
				this.set(!this.get());
			}
		}
	}
</script>
