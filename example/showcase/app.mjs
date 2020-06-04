"use strict"

import Vue from "vue"
import App from "./app.vue"
import Router from "bzd/vue/router/router.mjs"
import Notification from "bzd/vue/notification.mjs"

Vue.use(Router);
Vue.use(Notification);

new Vue({
	el: '#app',
	render: (h) => h(App)
});
