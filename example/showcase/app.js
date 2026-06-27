import Notification from "#bzd/nodejs/vue/notification.js";
import Router from "#bzd/nodejs/vue/router/router.js";
import { createApp } from "vue";

import App from "./app.vue";

const app = createApp(App);

app.use(Router);
app.use(Notification);

app.mount("#app");
