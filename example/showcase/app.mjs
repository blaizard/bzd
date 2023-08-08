import Notification from "bzd/vue/notification.mjs";
import Router from "bzd/vue/router/router.mjs";
import Vue from "vue";

import App from "./app.vue";

Vue.use(Router);
Vue.use(Notification);

new Vue({
  el : "#app",
  render : (h) => h(App),
});
