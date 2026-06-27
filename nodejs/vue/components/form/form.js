export class FormValues {
	constructor(values) {
		this.values = values;
	}

	/// Get the raw values.
	get() {
		return this.values;
	}

	/// Create a form data to be sent.
	getFormData() {
		const formData = new FormData();
		let updatedValues = {};
		let uid = 0;

		for (const [name, value] of Object.entries(this.values)) {
			// Note, we serialize twice the data structure here but I don't think it matters.
			// This is the client size, and it does not have performance constraints like the server has.
			updatedValues[name] = JSON.parse(
				JSON.stringify(value, (_, nestedValue) => {
					let uidBlob = null;
					if (nestedValue instanceof File) {
						uidBlob = "@bzd-file-" + ++uid + "-" + nestedValue.name;
					} else if (nestedValue instanceof Blob) {
						uidBlob = "@bzd-blob-" + ++uid + "-blob";
					} else {
						return nestedValue;
					}
					formData.append(name, nestedValue, uidBlob);
					return uidBlob;
				}),
			);
		}

		formData.append("@bzd-form", JSON.stringify(updatedValues));

		return formData;
	}
}
