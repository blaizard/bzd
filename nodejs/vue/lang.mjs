import { ref } from "vue";

import LocalStorage from "#bzd/nodejs/core/localstorage.mjs";
import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("lang");

export default {
	install: (app, dictionaries = {}) => {
		let lang = ref(null);
		let dictionary = ref({});

		const fetchDictionary = async (l) => {
			Exception.assert(l in dictionaries, "Missing dictionary for '{}'.", l);
			return (await dictionaries[l]()).default;
		};

		const getLang = () => {
			return lang.value;
		};

		const setLang = async (l) => {
			lang.value = l;
			LocalStorage.set("bzd-lang", lang.value);
			dictionary.value = await fetchDictionary(lang.value);
		};

		const get = (id) => {
			return dictionary.value[id] || "";
		};

		const getCapitalized = (id) => {
			const str = get(id);
			return str.charAt(0).toUpperCase() + str.slice(1);
		};

		setLang(LocalStorage.get("bzd-lang", "gb"));

		app.config.globalProperties.$lang = {
			getLang,
			setLang,
			get,
			getCapitalized,
		};
	},
};
