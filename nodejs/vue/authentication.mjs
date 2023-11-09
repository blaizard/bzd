export default {
	install(Vue, authentication) {
		Vue.prototype.$authentication = new Vue({
			data: {
				isAuthenticated: false,
				session: null,
			},
		});

		Vue.prototype.$makeAuthenticationURL = (url) => authentication.makeAuthenticationURL(url);

		authentication.options.onAuthentication = (maybeSession) => {
			Vue.prototype.$authentication.isAuthenticated = Boolean(maybeSession);
			Vue.prototype.$authentication.session = maybeSession;
		};

		// Trigger to get the session status.
		authentication.getSession();
	},
};
