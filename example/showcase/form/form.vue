<script>
import Template from "./template.vue";

export default {
	mixins: [Template],
	data: function () {
		return {
			value: {
				table: [
					{ value: 12, key: "Hello", price: 42 },
					{ value: 8, key: "World", price: 4.54 },
					{ value: -8, key: "Great", price: 0.2 },
				],
			},
		};
	},
	computed: {
		formDescription() {
			return [
				{ type: "Input", name: "input", caption: "Input" },
				{ type: "Password", name: "password", caption: "Password" },
				{ type: "Checkbox", name: "checkbox", caption: "Checkbox", text: "My Text" },
				{ type: "Textarea", name: "textarea", caption: "Textarea" },
				{ type: "Dropdown", name: "dropdown", caption: "Dropdown", list: ["Hello", "World"] },
				{
					type: "Autocomplete",
					name: "autocomplete",
					caption: "Autocomplete",
					list: async (text) => {
						return new Promise((resolve) => {
							setTimeout(() => {
								resolve(["You typed '" + text + "'", "delay 1000ms"]);
							}, 1000);
						});
					},
				},
				{ type: "Array", name: "array", caption: "Array" },
				{
					type: "Table",
					caption: "Table",
					name: "table",
					template: [
						{ type: "Input", caption: "Value", name: "value" },
						{ type: "Input", caption: "Key", name: "key" },
						{ type: "Input", caption: "Price", name: "price" },
					],
				},
				{ type: "File", caption: "File", name: "file", upload: "/dummy" },
				{ type: "Carousel", caption: "Carousel", name: "carousel", list: ["Hello", "World", "Great"] },
				{ type: "Editor", name: "editor", caption: "Editor" },
			];
		},
	},
};
</script>
