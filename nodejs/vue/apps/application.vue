<template>
	<Layout>
		<template #header>
			<RouterLink v-if="$metadata.title" link="/">
				<img v-if="$metadata.icon" class="bzd-apps-title-icon" :src="$metadata.icon" /> {{ $metadata.title }}
			</RouterLink>
		</template>
		<template #actions>
			<slot name="actions"></slot>
			<template v-if="$bzdAppsFeatures.authentication">
				<template v-if="$authentication.isAuthenticated">
					<!-- These are features that requires authentication -->
					<template v-if="$bzdAppsFeatures.services">
						<MenuEntry text="Services" icon="bzd-icon-tile" link="/services"></MenuEntry>
					</template>
					<template v-if="$bzdAppsFeatures.statistics">
						<MenuEntry text="Statistics" icon="bzd-icon-statistics" link="/statistics"></MenuEntry>
					</template>
					<template v-if="$bzdAppsFeatures.logger">
						<MenuEntry text="Logs" icon="bzd-icon-logs" link="/logger"></MenuEntry>
					</template>
					<MenuEntry v-if="$authentication.hasLogout()" text="Logout" icon="bzd-icon-close" link="/logout"></MenuEntry>
				</template>
				<template v-else>
					<MenuEntry text="Login" icon="bzd-icon-user" link="/login"></MenuEntry>
				</template>
			</template>
		</template>
		<template #content>
			<slot name="content">
				<RouterComponent name="view" class="bzd-apps-router-view"></RouterComponent>
			</slot>
		</template>
	</Layout>
</template>

<script>
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";
	import config from "#bzd/nodejs/vue/apps/config.json" with { type: "json" };

	export default {
		components: {
			Layout,
			MenuEntry,
		},
		mounted() {
			let routes = [];
			if (this.$bzdAppsFeatures.services) {
				routes.push({ path: "/services", component: () => import("./views/services.vue") });
			}
			if (this.$bzdAppsFeatures.statistics) {
				routes.push({ path: "/statistics", component: () => import("./views/statistics.vue") });
			}
			if (this.$bzdAppsFeatures.logger) {
				routes.push({ path: "/logger", component: () => import("./views/logger.vue") });
			}
			if (this.$bzdAppsFeatures.authentication) {
				routes.push({
					path: "/login",
					handler: async () => {
						await this.$rest.invoke("login", config.application);
					},
				});
				routes.push({
					path: "/logout",
					handler: async () => {
						await this.$rest.logout();
					},
				});
			}
			this.$router.set({
				// The view component router is where the main application content is displayed.
				component: "view",
				routes: routes,
				fallback: { component: () => import("./views/404.vue") },
			});
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
		$bzdIconNames: tile user close statistics logs
	);
</style>

<style lang="scss" scoped>
	.bzd-apps-title-icon {
		height: 1em;
	}
	.bzd-apps-router-view {
		padding: 20px;
	}
</style>
