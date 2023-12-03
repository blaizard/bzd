import { createApp } from "vue";

import App from "#bzd/apps/accounts/example/static/app.vue";
import APIPlugin from "#bzd/nodejs/vue/api.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";

const app = createApp(App);

// ---- Authentication ----

const authentication = new Authentication({
	unauthorizedCallback: async (needAuthentication) => {
		console.log("Unauthorized!", needAuthentication);
	},
});
app.use(AuthenticationPlugin, {
	authentication: authentication,
});

// ---- API ----

app.use(APIPlugin, {
	host: "http://127.0.0.1:8080",
	schema: APIv1,
	authentication: authentication,
	plugins: [authentication],
});

app.mount("#app");
