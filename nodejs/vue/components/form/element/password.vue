<script>
	import Input from "./input.vue";

	export default {
		mixins: [Input],
		computed: {
			/// ---- OVERRIDE Input ---------------
			mask() {
				return true;
			},
			html() {
				return true;
			},
			/// ---- IMPLEMENTATION ----------------------------------
			valueType() {
				return "string";
			},
			format() {
				return (value) => [...value].reduce((str) => str + "*", "");
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/styles/default/css/form/password.scss" as *;
</style>
