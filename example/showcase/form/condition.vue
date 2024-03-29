<script>
	import Template from "./template.vue";

	export default {
		mixins: [Template],
		data: function () {
			return {
				value: {
					condition: "function",
				},
			};
		},
		computed: {
			formDescription() {
				return [
					{ type: "Dropdown", caption: "Condition", name: "condition", list: ["function", "validation"] },
					{ type: "Input", caption: "Triggered by a function", condition: (value) => value.condition == "function" },
					{
						type: "Input",
						caption: "Triggered by a validation object",
						condition: { condition: "values(validation)" },
					},
				];
			},
		},
	};
</script>
