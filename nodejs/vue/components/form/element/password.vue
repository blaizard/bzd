<script>
"use strict";

import Input from "./input.vue";

export default {
	mixins: [Input],
	data: function() {
		return {
			mask: (value) => [...value].reduce((str) => str + "<i class=\"bzd-icon-password\"></i>", ""),
			html: true
		};
	},
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/password.scss";
</style>
