"use strict"

import Vue from "vue"
import App from "./app.vue"
import Router from "[bzd]/vue/router/router.mjs"

Vue.use(Router);

new Vue({
	el: '#app',
	render: (h) => h(App)
});
