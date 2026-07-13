export default class UnitHandler {
	constructor(unit) {
		this.unit = unit;
	}

	process(fragments) {
		for (const fragment of fragments.all()) {
			fragment.options.unit = this.unit;
		}
	}
}
