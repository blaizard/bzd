import { createApp } from "vue";

import API from "#bzd/nodejs/vue/api.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";

import APIv1 from "../api.v1.json" assert { type: "json" };

import App from "./app.vue";

const app = createApp(App);

app.use(Router, {
	hash: false,
});
app.use(API, {
	schema: APIv1,
});
app.use(Notification);

app.mount("#app");
