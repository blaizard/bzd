import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("lang");

export default {
	install(Vue, dictionaries = {}) {
		Vue.prototype.$lang = new Vue({
			data: {
				lang: null,
				dictionary: {}
			},
			methods: {
				async setLang(lang) {
					this.lang = lang;
					this.dictionary = await this.fetchDictionary(lang);
				},
				async fetchDictionary(lang) {
					Exception.assert(lang in dictionaries, "Missing dictionary for '{}'.", lang);
					return (await dictionaries[lang]()).default;
				},
				get(id) {
					return this.dictionary[id] || "";
				},
				getCapitalized(id) {
					const str = this.get(id);
					return str.charAt(0).toUpperCase() + str.slice(1);
				}
			}
		});
		Vue.prototype.$lang.setLang("en");
	}
};
