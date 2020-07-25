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
					type: "Input",
					name: "input",
					caption: "Input (positive integer)",
					validation: "mandatory type(integer) min(0)",
				},
				{ type: "Input", name: "email", caption: "Input (email)", validation: "mandatory email" },
				{ type: "Input", caption: "Input (same(email))", validation: "mandatory same(email)" },
				{ type: "Button", action: "approve" },
			];
		},
	},
};
</script>
