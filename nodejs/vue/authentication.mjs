import { reactive, computed } from "vue";

export default {
	install(app, options) {
		const state = reactive({
			session: null,
			isAuthenticated: computed(() => Boolean(state.session)),
		});

		// Setup
		options.authentication.options.onAuthentication = (maybeSession) => {
			if (maybeSession) {
				state.session = maybeSession;
			} else {
				state.session = null;
			}
		};

		// Trigger to get the session status.
		options.authentication.getSession();

		// API
		const $authentication = reactive({
			isAuthenticated: computed(() => state.isAuthenticated),
			hasScope: (scope) => state.isAuthenticated && state.session.matchAnyScopes(scope),
			makeUrl: (url) => options.authentication.makeAuthenticationURL(url),
		});

		app.config.globalProperties.$authentication = $authentication;
		app.provide("$authentication", $authentication);
	},
};
