export default {
	install(Vue, args) {
		Vue.prototype.$authentication = new Vue({
			data: {
				session: null,
				scopes: {},
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
				Vue.prototype.$authentication.scopes = maybeSession.getScopes().reduce((obj, scope) => {
					obj[scope] = true;
					return obj;
				}, {});
			} else {
				Vue.prototype.$authentication.session = null;
				Vue.prototype.$authentication.scopes = {};
			}
		};

		// Trigger to get the session status.
		args.authentication.getSession();
	},
};
