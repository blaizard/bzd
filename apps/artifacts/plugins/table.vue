<template>
	<div>
		Table works!!!
		<br />
		item: {{ item }}
		<br />
		innerItemList: {{ innerItemList }}
	</div>
</template>

<script>
	import Base from "./base.vue";

	export default {
		mixins: [Base]
	};
</script>
