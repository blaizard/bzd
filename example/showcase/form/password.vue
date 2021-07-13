<script>
	import Template from "./template.vue";

	export default {
		mixins: [Template],
		data: function() {
			return {
				value: {
					password: "hello"
				}
			};
		},
		computed: {
			formDescription() {
				return [
					{ type: "Password", caption: "Password" },
					{ type: "Password", name: "password", caption: "Password (value)" },
					{ type: "Password", caption: "Password (pre & post)", pre: "-->", post: "<--" },
					{ type: "Password", caption: "Password (placeholder)", placeholder: "Type Password..." }
				];
			}
		}
	};
</script>
