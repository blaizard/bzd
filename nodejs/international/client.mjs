export default class International {
	// Return a tuple of the language and country.
	static get() {
		const url = new URL(window.location.href);
		const maybeIntlFromUrl = url.searchParams.get("intl");
		if (maybeIntlFromUrl) {
			return maybeIntlFromUrl.split("-");
		}
		const maybeIntl = navigator.languages[0];
		if (maybeIntl) {
			const [language, country] = maybeIntl.toLowerCase().split("-");
			International.set(language, country);
			return [language, country];
		}
		International.set("en", "us");
		return ["en", "us"];
	}

	static set(language, country) {
		const url = new URL(window.location.href);
		url.searchParams.set("intl", language + "-" + country);
		window.history.replaceState({}, null, url.href);
	}
}
