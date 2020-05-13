import API from "../core/api.mjs";

export default {
	install(Vue, args) {
		const api = new API(args.schema, args.unauthorizedCallback);
		Vue.prototype.$api = api;
	}
};
