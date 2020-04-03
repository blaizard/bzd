<script>
	"use strict";

	import Dropdown from "./dropdown.vue"

	export default {
		mixins: [Dropdown],
		data: function() {
			return {
				posthtml: false,
				editable: true
			};
		},
		computed: {
			containerClass() {
				return {
					"irform-autocomplete": true,
					"irform-loading": this.loading
				};
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
