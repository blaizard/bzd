<script>
    import Template from "./template.vue"; 

	export default {
        mixins: [Template],
		data: function () {
			return {
                value: {
					dropdown: "Hello"
				},
				dropdownList: []
			}
		},
		mounted() {
			setTimeout(() => {
				this.dropdownList = [ "I", "am", "a", "lazy", "son", "of", "a", "b***h!"];
			}, 100);
		},
        computed: {
            formDescription() {
                return [
					{ type: "Dropdown", caption: "Dropdown", list: ["Hello", "World"] },
					{ type: "Dropdown", caption: "Dropdown (value)", name: "dropdown", list: ["Hello", "World"] },
					{ type: "Dropdown", caption: "Dropdown (lazy content)", list: this.dropdownList },
					{ type: "Dropdown", caption: "Dropdown (long)", list: Array.from(Array(100).keys()) },
					{ type: "Dropdown", caption: "Dropdown (placeholder)",  list: ["Hello", "World"], placeholder: "Enter text..." },
					{ type: "Dropdown", caption: "Dropdown (disabled)", disable: true, list: ["Hello", "World"] },
					{ type: "Dropdown", caption: "Dropdown (editable)", list: {hello: "Hello", world: "World"}, editable: true },
					{ type: "Dropdown", caption: "Dropdown (html)", list: {hello: "<b>Hel</b><i>lo</i>", world: "<span style=\"color: red;\">World</span>"}, html: true },
					{ type: "Dropdown", caption: "Dropdown (html, multi)", list: {hello: "<b>Hel</b><i>lo</i>", world: "<span style=\"color: red;\">World</span>"}, html: true, multi: true },
					{ type: "Dropdown", caption: "Dropdown (editable, multi)", list: {hello: "Hello", world: "World"}, editable: true, multi: true, placeholder: "Type text here..." },
					{ type: "Dropdown", caption: "Dropdown (fct callback)", list: (text) => ["Hello World!", "You typed '" + text + "'"], editable: true },
					{ type: "Dropdown", caption: "Dropdown (fct callback delay 500ms + 1000ms)", list: async (text) => {
						return new Promise((resolve) => {
							setTimeout(() => {
								resolve(["You typed '" + text + "'", "delay 500ms + 1000ms"]);
							}, 1000);
						});
					}, delay: 500, editable: true }
				]
            },
        }
	}
</script>
