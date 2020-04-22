import API from "../core/api.js";

export default {
	install(Vue, apiDescription) {
        const api = new API(apiDescription);
		Vue.prototype.$api = api;
    }
}
