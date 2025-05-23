<template>
	<Layout :full-page="true">
		<template #header>
			<RouterLink v-if="$metadata.title" link="/">
				<img v-if="$metadata.icon" class="bzd-title-icon" :src="$metadata.icon" /> {{ $metadata.title }}
			</RouterLink>
		</template>
		<template #actions>
			<slot name="actions"></slot>
			<template v-if="$authentication">
				<template v-if="$authentication.isAuthenticated">
					<MenuEntry v-if="$authentication.hasLogout()" text="Logout" icon="bzd-icon-close" link="/logout"></MenuEntry>
				</template>
				<template v-else>
					<MenuEntry text="Login" icon="bzd-icon-user" link="/login"></MenuEntry>
				</template>
			</template>
		</template>
		<template #content>
			<slot name="content"></slot>
		</template>
	</Layout>
</template>

<script>
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout,
			MenuEntry,
		},
		methods: {
			handleHeader() {
				this.$router.dispatch("/");
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/styles/default/css/base.scss" as *;
	@use "@/nodejs/styles/default/css/tooltip.scss" as *;
	@use "@/nodejs/styles/default/css/loading.scss" as *;

	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: tile user close
	);
</style>

<style lang="scss" scoped>
	.bzd-title-icon {
		height: 1em;
		filter: invert(0);
	}
</style>
