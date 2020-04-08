<template>
  <Layout>
    <template v-slot:header>Dashboard</template>
    <template v-slot:actions>
      <ul>
        <li><i class="bzd-icon-menu"></i> Hello</li>
      </ul>
    </template>
    <template v-slot:menu>
      <ul>
        <li><span><i class="bzd-icon-menu"></i> About</span></li>
      </ul>
    </template>
    <template v-slot:content>
      Content
    </template>
    <template v-slot:footer>
      Footer
    </template>
  </Layout>
</template>

<script>
	"use strict"

	import Layout from "./layout.vue";

  export default {
		components: {
			Layout
		}
  }
</script>

<style lang="scss">
	@import "~[bzd]/assets/style/base.scss";

  $bzdIconNames: menu;
  @import "~[bzd]/icons.scss";
</style>
