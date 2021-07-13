import Vue from "vue";
import Router from "bzd/vue/router/router.mjs";
import API from "bzd/vue/api.mjs";
import Notification from "bzd/vue/notification.mjs";

import App from "./app.vue";
import APIv1 from "../api.v1.json";

Vue.use(Router, {
	hash: false
});
Vue.use(API, {
	schema: APIv1
});
Vue.use(Notification);

new Vue({
	el: "#app",
	render: (h) => h(App)
});
