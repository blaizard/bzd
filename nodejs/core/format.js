"use strict";

/**
 * Format a string using python format syntax.
 * @param {*} str 
 * @param  {...any} args 
 */
export default function (str, ...args) {

	let pattern = new RegExp("{([^}:]*)(?::([^}]*))?}", "g");
	let output = "";
	let index = 0;
	let substitutionIndex = 0;
	let m = null;
	do {
		m = pattern.exec(str);

		// Copy the string that has not been processed and update the current index
		output += str.substring(index, ((m) ? m.index : undefined));
		index = (m) ? (m.index + m[0].length) : 0;

		// Handle the operation if any match
		if (m) {
			const format = _parseFormat(m[1], m[2], substitutionIndex);
			output += args[format.index];

			++substitutionIndex;
		}
	} while (m);

	return output;
}

function _parseFormat(substitutionIndex, options, currentSubstitutionIndex) {
	return {
		index: (substitutionIndex) ? parseInt(substitutionIndex) : currentSubstitutionIndex
	}
}
