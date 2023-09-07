<template>
	<div>
		<template v-for="(item, index) in innerItemList" :key="index">
			<h2>{{ item.name }}</h2>
			<ViewCategory :pathList="pathList.concat([item.name])"></ViewCategory>
		</template>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import ViewCategory from "#bzd/apps/artifacts/plugins/bzd/view_category.vue";

	export default {
		mixins: [Base],
		components: {
			ViewCategory,
		},
	};
</script>
