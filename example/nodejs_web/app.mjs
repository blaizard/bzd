import VueCompositionApi from "@vue/composition-api";
import Vue from "vue";

import App from "./app.vue";

Vue.use(VueCompositionApi);

new Vue({
  el : "#app",
  render : (h) => h(App),
});
