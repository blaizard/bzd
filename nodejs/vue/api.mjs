"use strict";

import API from "../core/api/client.mjs";

export default {
	install(Vue, args) {
		const api = new API(args.schema, args);
		Vue.prototype.$api = api;
	}
};
