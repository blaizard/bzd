export default class ExpiresHandler {
	constructor(expires) {
		this.expires = expires;
	}

	process(fragments) {
		for (const fragment of fragments.all()) {
			fragment.options.expires = this.expires;
		}
	}
}
