"use strict"

import Vue from "vue"
import Router from "bzd/vue/router/router.mjs"
import API from "bzd/vue/api.mjs";

import App from "./app.vue"
import APIv1 from "../api.v1.json";

Vue.use(Router);
Vue.use(API, APIv1);

new Vue({
	el: '#app',
	render: (h) => h(App),
});
