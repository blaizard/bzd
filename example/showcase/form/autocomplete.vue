<script>
	import Template from "./template.vue";

	export default {
		mixins: [Template],
		data: function () {
			return {
				value: {},
			};
		},
		computed: {
			formDescription() {
				return [
					{
						type: "Autocomplete",
						caption: "Autocomplete",
						list: async (text) => {
							return new Promise((resolve) => {
								setTimeout(() => {
									resolve(["You typed '" + text + "'", "delay 1000ms"]);
								}, 1000);
							});
						},
					},
					{ type: "Autocomplete", caption: "Autocomplete (disable)", disable: true },
				];
			},
		},
	};
</script>
