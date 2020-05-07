<script>
"use strict";

import Dropdown from "./dropdown.vue";

export default {
	mixins: [Dropdown],
	data: function() {
		return {
			posthtml: false,
			editable: true
		};
	},
	computed: {
		className() {
			return "irform-autocomplete";
		}
	},
	methods: {
		async processList(text) {
			// If the input text is empty, do nothing
			if (!text) {
				this.list = this.filteredList = [];
				return;
			}
			// Call parent method
			Dropdown.methods.processList.call(this, text);
		}
	}
};
</script>

<style lang="scss">
	@import "~[bzd-style]/css/form/autocomplete.scss";
</style>
