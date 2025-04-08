/// Format a string using python format syntax.
/// @param {*} str
/// @param  {...any} args
export default function (str, ...args) {
	// By default false, as it gets updated during the processing part.
	let pattern = new RegExp("{([^}:]*)(?::([^}]*))?}", "g");
	let substitutionIndex = 0;

	const replacer = (_, name, metadata) => {
		// Handle the operation if any match
		const format = _parseFormat(name, metadata || "", substitutionIndex);
		++substitutionIndex;

		// Sync the match with the args
		let value = null;
		if (typeof format.index == "string") {
			const a = args[0];
			if (!a || a.constructor !== Object) {
				throw new Error(
					"Expected a dictionary as argument to match key '" + format.index + "' while formatting string: " + str,
				);
			}
			if (!(format.index in a)) {
				throw new Error("Missing key '" + format.index + "' while formatting string: " + str);
			}
			value = a[format.index];
		} else {
			if (format.index >= args.length) {
				throw new Error("Too few arguments passed while formatting string: " + str);
			}
			value = args[format.index];
		}

		switch (format.metadata || "") {
			case "j":
				// Handle Set, otherwise they will be rendered as '{}'.
				return JSON.stringify(value, (_, value) => (value instanceof Set ? [...value] : value));
			case "":
				return String(value);
			default:
				// Round the float with a specific precision.
				if (format.metadata.startsWith(".")) {
					const precision = parseInt(format.metadata.substring(1));
					return String(value.toFixed(precision));
				}
				// Pad the number with leading zeros.
				else if (format.metadata.startsWith("0")) {
					const number = parseInt(format.metadata.substring(1));
					return String(value).padStart(number, "0");
				}
		}
		throw new Error("Unsupported formatting metadata: " + format.metadata);
	};

	return str.replace(pattern, replacer);
}

function _parseFormat(substitutionIndex, metadata, currentSubstitutionIndex) {
	const index = substitutionIndex ? parseInt(substitutionIndex) : currentSubstitutionIndex;
	return {
		index: isNaN(index) ? substitutionIndex : index,
		metadata: metadata,
	};
}
