<template>
  <Layout>
    <template v-slot:header>Dashboard</template>
    <template v-slot:actions>
      <ul>
        <li><i class="bzd-icon-configuration" v-tooltip="{'text': 'Configuration'}"></i> Configuration</li>
      </ul>
    </template>
    <template v-slot:menu>
      <ul>
        <li><span><i class="bzd-icon-menu"></i> About</span></li>
      </ul>
      <router-link to="/add">Go to Frr</router-link>
      <router-link to="/dashboard">Go to Bar</router-link>
    </template>
    <template v-slot:content>
      <span v-tooltip="{'text': 'Hello world'}">Content</span>
      <router-view></router-view>
    </template>
    <template v-slot:footer>
      Footer
    </template>
  </Layout>
</template>

<script>
	"use strict"

	import Layout from "[frontend]/layout.vue";
  import DirectiveTooltip from "[bzd]/vue/directives/tooltip.js"

  export default {
		components: {
			Layout
		},
    directives: {
      "tooltip": DirectiveTooltip
    }
  }
</script>

<style lang="scss">
	@import "~[bzd]/assets/style/base.scss";
	@import "~[bzd]/assets/style/tooltip.scss";

  $bzdIconNames: configuration;
  @import "~[bzd]/icons.scss";
</style>
