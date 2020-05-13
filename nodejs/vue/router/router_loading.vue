<template>
	<div v-loading="true">&nbsp;</div>
</template>

<script>
"use strict";

import DirectiveLoading from "bzd/vue/directives/loading.mjs";

export default {
	directives: {
		"loading": DirectiveLoading
	}
};
</script>
