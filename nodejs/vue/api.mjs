import API from "../core/api.mjs";

export default {
	install(Vue, apiDescription) {
		const api = new API(apiDescription);
		Vue.prototype.$api = api;
	}
};
