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
				return [{ type: "File", caption: "Add a single file" }];
			},
		},
	};
</script>
