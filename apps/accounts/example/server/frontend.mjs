import { createApp } from "vue";

import App from "#bzd/apps/accounts/example/server/frontend.vue";
import APIPlugin from "#bzd/nodejs/vue/api.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import Authentication from "#bzd/apps/accounts/authentication/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Config from "#bzd/apps/accounts/example/config.json" assert { type: "json" };

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

// ---- API ----

app.use(APIPlugin, {
	schema: APIv1,
	authentication: authentication,
	plugins: [authentication],
});

app.mount("#app");
