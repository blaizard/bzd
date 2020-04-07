<template>
  <Layout>
    <template v-slot:header>Dashboard</template>
    <!--<template v-slot:quick>
      <ul>
        <li><i></i></li>
        <li>
      </ul>
    </template>//-->
    <template v-slot:menu>
      <ul>
        <li>About</li>
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
</style>
