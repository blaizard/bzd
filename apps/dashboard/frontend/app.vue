<template>
	<Layout>
		<template #header>Dashboard</template>
		<template #actions>
			<MenuEntry text="Add new tile" icon="bzd-icon-add" link="/new"></MenuEntry>
			<MenuEntry v-if="!edit" text="Edit" icon="bzd-icon-edit" @click="handleEdit"></MenuEntry>
			<MenuEntry v-else text="OK" icon="bzd-icon-check" @click="handleEdit"></MenuEntry>
		</template>
		<template #menu>
			<MenuEntry text="About" icon="bzd-icon-edit">
				<MenuEntry text="One" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Two" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Three" icon="bzd-icon-edit"></MenuEntry>
			</MenuEntry>
		</template>
		<template #content>
			<span v-tooltip="{'text': 'Hello world'}">Content</span>
			<RouterComponent ref="view" :edit="edit" another="yes"></RouterComponent>
			<RouterLink link="/">dashboard</RouterLink>
		</template>
		<template #footer>
			Footer <button>Test</button>
		</template>
	</Layout>
</template>

<script>
	"use strict"

	import Layout from "[frontend]/layout.vue";
	import MenuEntry from "[frontend]/menu_entry.vue";
	import DirectiveTooltip from "[bzd]/vue/directives/tooltip.js"

	export default {
		components: {
			Layout, MenuEntry
		},
		directives: {
			"tooltip": DirectiveTooltip
		},
		data: function () {
			return {
				edit: false
			}
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [
                    { path: '/', component: () => import("[frontend]/tiles.vue") },
                    { path: '/new', component: () => import("[frontend]/config.vue") },
                    { path: '/update/{uid}', component: () => import("[frontend]/config.vue") },
				],
				fallback: { component: () => import("[frontend]/404.vue") }
			});
		},
		methods: {
			handleEdit() {
				this.edit = true;
			},
		}
	}
</script>

<style lang="scss">
	@import "~[bzd-style]/css/base.scss";
	@import "~[bzd-style]/css/tooltip.scss";

	$bzdIconNames: edit, add, check;
	@import "~[bzd]/icons.scss";

    @import "~[bzd-style]/css/colors.scss";
    .bzd-dashboard-color-red {
        background-color: $bzdGraphColorRed;
        color: $bzdGraphColorWhite;
    }
    .bzd-dashboard-color-blue {
        background-color: $bzdGraphColorBlue;
        color: $bzdGraphColorWhite;
    }
    .bzd-dashboard-color-orange {
        background-color: $bzdGraphColorOrange;
        color: $bzdGraphColorWhite;
    }
    .bzd-dashboard-color-yellow {
        background-color: $bzdGraphColorYellow;
        color: $bzdGraphColorBlack;
    }
    .bzd-dashboard-color-green {
        background-color: $bzdGraphColorGreen;
        color: $bzdGraphColorWhite;
    }
</style>
