"use strict";

export default {
	install(Vue, authentication) {
		Vue.prototype.$authentication = authentication;
	}
};
