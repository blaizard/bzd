import API from "#bzd/nodejs/vue/api.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";
import Vue from "vue";

import APIv1 from "../api.v1.json" assert { type: "json" };

import App from "./app.vue";

Vue.use(Router, {
	hash: false,
});
Vue.use(API, {
	schema: APIv1,
});

new Vue({
	el: "#app",
	render: (h) => h(App),
});
