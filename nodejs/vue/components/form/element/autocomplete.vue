<template>
	<Autocomplete class="irform-autocomplete"
			:description="descriptionDropdown"
			:value="value"
			@input="$emit('input', $event)">
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
		}
	}
};

export default {
	components: {
		Autocomplete
	},
	props: {
		value: {type: String, required: false, default: ""},
		description: {type: Object, required: false, default: () => ({})}
	},
	computed: {
		descriptionDropdown() {
			return Object.assign({}, this.description, {
				editable: true,
			});
		}
	},
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/autocomplete.scss";
</style>
