import Format from "../core/format.mjs";

function _unitsToString(unitList, value, maxNbUnits = 1, decimalPoints = 1) {
	let unitIndex = 0;
	while (value > unitList[unitIndex][2]) {
		if (!unitList[unitIndex + 1]) {
			break;
		}
		value /= unitList[unitIndex][2];
		unitIndex++;
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
		/*maxNbUnits*/ 1,
		decimalPoints,
	);
}

export function timeMsToString(value, maxNbUnits = 1) {
	return _unitsToString(
		[
			["ms", "ms", 1000],
			["s", "s", 60],
			[" min", " min", 60],
			["h", "h", 24],
			[" day", " days", 1],
		],
		value,
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
