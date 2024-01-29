/**
 * Format a string using python format syntax.
 * @param {*} str
 * @param  {...any} args
 */
export default function (str, ...args) {
	// By default false, as it gets updated during the processing part.
	let pattern = new RegExp("{([^}:]*)(?::([^}]*))?}", "g");
	let output = "";
	let index = 0;
	let substitutionIndex = 0;
	let m = null;
	do {
		m = pattern.exec(str);

		// Copy the string that has not been processed and update the current index
		output += str.substring(index, m ? m.index : undefined);
		index = m ? m.index + m[0].length : 0;

		// Handle the operation if any match
		if (m) {
			const format = _parseFormat(m[1], m[2], substitutionIndex);
			const value = typeof format.index == "string" ? args[0][format.index] : args[format.index];

			switch (format.type || "") {
				case "j":
					output += JSON.stringify(value);
					break;
				case "":
					output += String(value);
					break;
				default:
					// Round the float with a specific precision.
					if (format.type.startsWith(".")) {
						const precision = parseInt(format.type.substring(1));
						output += String(value.toFixed(precision));
						break;
					}
					// Pad the number with leading zeros.
					else if (format.type.startsWith("0")) {
						const number = parseInt(format.type.substring(1));
						output += String(value).padStart(number, "0");
						break;
					}
					throw new Error("Unsupported formatting type: " + format.type);
			}

			++substitutionIndex;
		}
	} while (m);

	return output;
}

function _parseFormat(substitutionIndex, options, currentSubstitutionIndex) {
	const index = substitutionIndex ? parseInt(substitutionIndex) : currentSubstitutionIndex;
	return {
		index: isNaN(index) ? substitutionIndex : index,
		type: options,
	};
}
