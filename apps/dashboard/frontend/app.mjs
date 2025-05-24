import { createApp } from "vue";

import RestPlugin from "#bzd/nodejs/vue/rest.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";

import APIv1 from "#bzd/api.json" with { type: "json" };

import App from "./app.vue";

const app = createApp(App);

app.use(Router, {
	hash: false,
});
app.use(RestPlugin, {
	schema: APIv1.rest,
});
app.use(Notification);

app.mount("#app");
