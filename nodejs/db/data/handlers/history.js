export default class HistoryHandler {
	constructor(history) {
		this.history = history;
	}

	process(fragments) {
		for (const fragment of fragments.all()) {
			fragment.options.history = this.history;
		}
	}
}
