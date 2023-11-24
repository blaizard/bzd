import API from "#bzd/nodejs/vue/api.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";
import { createApp } from "vue";

import APIv1 from "../api.v1.json" assert { type: "json" };

import App from "./app.vue";

const app = createApp(App);

app.use(Router, {
	hash: false,
});
app.use(API, {
	schema: APIv1,
});

app.mount("#app");
