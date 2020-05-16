"use strict";

export default {
	install(Vue, authentication) {

		Vue.prototype.$authentication = new Vue({
			data: {
				isAuthenticated: false
			}
		});

		authentication.options.onAuthentication = (isAuthenticated) => {
			Vue.prototype.$authentication.isAuthenticated = isAuthenticated;
		};
	}
};
