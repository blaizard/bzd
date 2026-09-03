import { createApp } from "vue";

import App from "#bzd/apps/accounts/example/serverless/app.vue";
import RestPlugin from "#bzd/nodejs/vue/rest.js";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Authentication from "#bzd/apps/accounts/authentication/client.js";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.js";
import { configAccounts } from "#bzd/apps/accounts/example/config_nodejs.js";

const app = createApp(App);

// ---- Authentication ----

const authentication = new Authentication({
	accounts: configAccounts(),
	unauthorizedCallback: async (needAuthentication) => {
		console.log("Unauthorized!", needAuthentication);
	},
});
app.use(AuthenticationPlugin, {
	authentication: authentication,
});

// ---- REST ----

app.use(RestPlugin, {
	host: configAccounts(),
	schema: APIv1.rest,
	authentication: authentication,
	plugins: [authentication],
});

app.mount("#app");
