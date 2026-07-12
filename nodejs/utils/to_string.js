import Format from "../core/format.js";

/// Formats a raw numeric value into a human-readable string using a hierarchical list of units.
///
/// \param unitList unitList An array of unit definitions, ordered from smallest to largest.
/// \param value value The raw numeric value to be converted.
/// \param startIndex The index in unitList corresponding to the input value's base unit.
/// \param maxNbUnits The maximum number of distinct unit components to include in the output string.
/// \param decimalPoints The number of decimal places to apply strictly to the final/smallest displayed unit.
///
/// \return {string} A space-separated, human-readable string representing the formatted value.
function _unitsToString(unitList, value, startIndex = 0, maxNbUnits = 1, decimalPoints = 1) {
	let unitIndex = startIndex;

	// Scale UP
	while (value > unitList[unitIndex][2]) {
		if (!unitList[unitIndex + 1]) {
			break;
		}
		value /= unitList[unitIndex][2];
		unitIndex++;
	}

	// Scale DOWN
	while (value > 0 && value < 1 && unitIndex > 0) {
		unitIndex--;
		value *= unitList[unitIndex][2];
	}

	let output = [];
	for (let i = maxNbUnits; i > 0 && unitIndex >= 0; --i) {
		output.push(
			parseFloat(value).toFixed(decimalPoints) + (value < 2 ? unitList[unitIndex][0] : unitList[unitIndex][1]),
		);
		value = (value - parseInt(value)) * unitList[unitIndex][2];
		unitIndex--;
	}

	return output.join(" ");
}

export function bytesToString(value, decimalPoints = 1) {
	return _unitsToString(
		[
			["B", "B", 1024],
			["KiB", "KiB", 1024],
			["MiB", "MiB", 1024],
			["GiB", "GiB", 1024],
			["TiB", "TiB", 1024],
		],
		value,
		/*startIndex*/ 0,
		/*maxNbUnits*/ 1,
		decimalPoints,
	);
}

export function frequencyToString(value, decimalPoints = 1) {
	return _unitsToString(
		[
			["Hz", "Hz", 1000],
			["KHz", "KHz", 1000],
			["MHz", "MHz", 1000],
			["GHz", "GHz", 1000],
			["THz", "THz", 1000],
		],
		value,
		/*startIndex*/ 0,
		/*maxNbUnits*/ 1,
		decimalPoints,
	);
}

/// Convert a time in seconds into a string.
export function timeToString(value, maxNbUnits = 1) {
	return _unitsToString(
		[
			["ns", "ns", 1000],
			["µs", "µs", 1000],
			["ms", "ms", 1000],
			["s", "s", 60],
			[" min", " min", 60],
			["h", "h", 24],
			[" day", " days", 1],
		],
		value,
		/*startIndex*/ 3,
		maxNbUnits,
		/*decimalPoints*/ 0,
	);
}

export function dateToString(format, timestamp) {
	const date = new Date(timestamp);
	return Format(format, {
		ms: date.getMilliseconds(),
		s: date.getSeconds(),
		min: date.getMinutes(),
		h: date.getHours(),
		d: date.getDate(),
		m: date.getMonth() + 1,
		y: date.getFullYear(),
	});
}

export function dateToDefaultString(timestamp) {
	return dateToString("{y:04}-{m:02}-{d:02} {h:02}:{min:02}:{s:02}.{ms}", timestamp);
}

export function capitalize(string) {
	return string.charAt(0).toUpperCase() + string.slice(1);
}
