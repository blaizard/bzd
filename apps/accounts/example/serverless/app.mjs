import { createApp } from "vue";

import App from "#bzd/apps/accounts/example/serverless/app.vue";
import RestPlugin from "#bzd/nodejs/vue/rest.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Authentication from "#bzd/apps/accounts/authentication/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Config from "#bzd/apps/accounts/example/config.json" with { type: "json" };

const app = createApp(App);

// ---- Authentication ----

const authentication = new Authentication({
	accounts: Config.accounts,
	unauthorizedCallback: async (needAuthentication) => {
		console.log("Unauthorized!", needAuthentication);
	},
});
app.use(AuthenticationPlugin, {
	authentication: authentication,
});

// ---- REST ----

app.use(RestPlugin, {
	host: Config.accounts,
	schema: APIv1.rest,
	authentication: authentication,
	plugins: [authentication],
});

app.mount("#app");
