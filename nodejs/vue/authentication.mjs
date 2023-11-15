export default {
	install(Vue, args) {
		Vue.prototype.$authentication = new Vue({
			data: {
				session: null,
			},
			methods: {
				hasScope(scope) {
					return this.isAuthenticated && this.session.matchAnyScopes(scope);
				},
			},
			computed: {
				isAuthenticated() {
					return Boolean(this.session);
				},
			},
		});

		Vue.prototype.$makeAuthenticationURL = (url) => args.authentication.makeAuthenticationURL(url);

		args.authentication.options.onAuthentication = (maybeSession) => {
			if (maybeSession) {
				Vue.prototype.$authentication.session = maybeSession;
			} else {
				Vue.prototype.$authentication.session = null;
			}
		};

		// Trigger to get the session status.
		args.authentication.getSession();
	},
};
