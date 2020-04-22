"use strict"

import Vue from "vue"
import App from "[frontend]/app.vue"
import Router from "[bzd]/vue/router/router.js"
import API from "[bzd]/vue/api.js";
import APIv1 from "[dashboard]/api.v1.json";

Vue.use(Router);
Vue.use(API, APIv1);

new Vue({
	el: '#app',
	render: (h) => h(App),
});
