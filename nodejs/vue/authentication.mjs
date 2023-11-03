export default {
	install(Vue, authentication) {
		Vue.prototype.$authentication = new Vue({
			data: {
				isAuthenticated: false,
			},
			async mounted() {
				this.isAuthenticated = await authentication.isAuthenticated();
			},
		});

		Vue.prototype.$makeAuthenticationURL = (url) => authentication.makeAuthenticationURL(url);

		authentication.options.onAuthentication = (isAuthenticated) => {
			Vue.prototype.$authentication.isAuthenticated = isAuthenticated;
		};
	},
};
