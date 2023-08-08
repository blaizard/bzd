import API from "bzd/vue/api.mjs";
import Notification from "bzd/vue/notification.mjs";
import Router from "bzd/vue/router/router.mjs";
import Vue from "vue";

import APIv1 from "../api.v1.json";

import App from "./app.vue";

Vue.use(Router, {
  hash : false,
});
Vue.use(API, {
  schema : APIv1,
});
Vue.use(Notification);

new Vue({
  el : "#app",
  render : (h) => h(App),
});
