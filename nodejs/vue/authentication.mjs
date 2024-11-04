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

		// API
		const $authentication = reactive({
			isAuthenticated: computed(() => state.isAuthenticated),
			hasScope: (scope) => state.isAuthenticated && state.session.getScopes().matchAny(scope),
			updateUrl: (url) => options.authentication.updateAuthenticationURL(url),
			updateFetch: (optionsFetch) => options.authentication.updateAuthenticationFetch(optionsFetch),
		});

		app.config.globalProperties.$authentication = $authentication;
		app.provide("$authentication", $authentication);
	},
};
