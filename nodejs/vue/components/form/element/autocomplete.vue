<template>
	<Autocomplete
		:class="containerClass"
		:description="descriptionDropdown"
		:model-value="modelValue"
		@update:model-value="$emit('update:model-value', $event)"
	>
	</Autocomplete>
</template>

<script>
	import Dropdown from "./dropdown.vue";

	const Autocomplete = {
		extends: Dropdown,
		methods: {
			async process(text) {
				// If the input text is empty, do nothing
				if (!text) {
					this.list = this.filteredList = [];
					return;
				}
				// Call parent method
				await Dropdown.methods.process.call(this, text);
			},
		},
	};

	export default {
		components: {
			Autocomplete,
		},
		props: {
			modelValue: { required: false, default: "" },
			description: { type: Object, required: false, default: () => ({}) },
		},
		computed: {
			containerClass() {
				return {
					"irform-autocomplete": true,
					[this.description["class"]]: true,
				};
			},
			descriptionDropdown() {
				return Object.assign({}, this.description, {
					editable: true,
				});
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/autocomplete.scss" as *;
</style>
