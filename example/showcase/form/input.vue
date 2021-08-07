<script>
	import Template from "./template.vue";

	export default {
		mixins: [Template],
		data: function() {
			return {
				value: {}
			};
		},
		computed: {
			formDescription() {
				return [
					{ type: "Input", caption: "Input" },
					{ type: "Input", caption: "Input (disabled)", disable: true },
					{ type: "Input", caption: "Input (placeholder)", placeholder: "Enter text..." },
					{ type: "Input", caption: "Input (pre)", pre: "$" },
					{ type: "Input", caption: "Input (post)", post: "€" },
					{ type: "Input", caption: "Input (format)", format: "***-***-***" },
					{ type: "Input", caption: "Input (multi)", multi: true },
					{
						type: "Input",
						caption: "Input (multi + format)",
						placeholder: "Type hello...",
						multi: true,
						format: { hello: "<b>Hel</b><i>lo</i>", world: '<span style="color: red;">World</span>' },
						html: true
					}
				];
			}
		}
	};
</script>
