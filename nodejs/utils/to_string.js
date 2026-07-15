import Format from "../core/format.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("to_string");

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

export class UnitToStringFactory {
	static presetUCUMMetrics = {
		prefixes: {
			y: 0,
			z: 1,
			a: 2,
			f: 3,
			p: 4,
			n: 5,
			u: 6,
			m: 7,
			c: 8,
			d: 9,
			"": 10,
			da: 11,
			h: 12,
			k: 13,
			M: 14,
			G: 15,
			T: 16,
			P: 17,
			E: 18,
			Z: 19,
			Y: 20,
		},
		unitList: [
			["y", "y", 1000],
			["z", "z", 1000],
			["a", "a", 1000],
			["f", "f", 1000],
			["p", "p", 1000],
			["n", "n", 1000],
			["μ", "μ", 1000],
			["m", "m", 10],
			["c", "c", 10],
			["d", "d", 10],
			["", "", 10],
			["da", "da", 10],
			["h", "h", 10],
			["k", "k", 1000],
			["M", "M", 1000],
			["G", "G", 1000],
			["T", "T", 1000],
			["P", "P", 1000],
			["E", "E", 1000],
			["Z", "Z", 1000],
			["Y", "Y", 1000],
		],
	};
	static presetUCUMMetricsNo10Forward = {
		prefixes: {
			"": 0,
			k: 1,
			M: 2,
			G: 3,
			T: 4,
			P: 5,
			E: 6,
			Z: 7,
			Y: 8,
		},
		unitList: [
			["", "", 1000],
			["k", "k", 1000],
			["M", "M", 1000],
			["G", "G", 1000],
			["T", "T", 1000],
			["P", "P", 1000],
			["E", "E", 1000],
			["Z", "Z", 1000],
			["Y", "Y", 1000],
		],
	};
	static presetUCUMBinary = {
		prefixes: {
			"": 0,
			Ki: 1,
			Mi: 2,
			Gi: 3,
			Ti: 4,
		},
		unitList: [
			["", "", 1024],
			["Ki", "Ki", 1024],
			["Mi", "Mi", 1024],
			["Gi", "Gi", 1024],
			["Ti", "Ti", 1024],
		],
	};
	static makeFromPreset(value, prefix, unit, preset, decimalPoints = 1) {
		Exception.assert(prefix in preset.prefixes, "Invalid metric unit prefix '{}'.", prefix);
		const startIndex = preset.prefixes[prefix];
		return _unitsToString(preset.unitList, value, startIndex, /*maxNbUnits*/ 1, decimalPoints) + unit;
	}
	static makeFormatterDictionary(schema) {
		const unitList = schema.map(([_, display]) => display);
		return Object.fromEntries(
			schema.map(([unit, _], index) => [
				unit,
				(value, decimalPoints = 1) => _unitsToString(unitList, value, index, /*maxNbUnits*/ 1, decimalPoints),
			]),
		);
	}
	static makePostFormatter(postProcess) {
		return (value, decimalPoints) => postProcess(value.toFixed(decimalPoints));
	}
	static makePreFormatter(preProcess) {
		return (value, decimalPoints) => preProcess(value).toFixed(decimalPoints);
	}
	static makeFormatter(preProcess, postProcess) {
		return (value, decimalPoints) => postProcess(preProcess(value).toFixed(decimalPoints));
	}
}

const UCUMToFormatter = Object.assign(
	{
		Cel: UnitToStringFactory.makePostFormatter((value) => value + "°C"),
		"%": UnitToStringFactory.makeFormatter(
			(value) => value * 100,
			(value) => value + "%",
		),
	},
	UnitToStringFactory.makeFormatterDictionary([
		["ys", ["ys", "ys", 1000]],
		["zs", ["zs", "zs", 1000]],
		["as", ["as", "as", 1000]],
		["fs", ["fs", "fs", 1000]],
		["ps", ["ps", "ps", 1000]],
		["ns", ["ns", "ns", 1000]],
		["us", ["μs", "μs", 1000]],
		["ms", ["ms", "ms", 1000]],
		["s", ["s", "s", 60]],
		["min", ["min", "min", 60]],
		["h", ["h", "h", 24]],
		["d", [" day", " days", 1]],
	]),
);
const UCUMToPresets = {
	Hz: UnitToStringFactory.presetUCUMMetricsNo10Forward,
	By: Object.assign({ unit: "B" }, UnitToStringFactory.presetUCUMBinary),
	g: UnitToStringFactory.presetUCUMMetrics,
};
const maxUCUMToPresetsLength = Math.max(...Object.keys(UCUMToPresets).map((unit) => unit.length));

/// Convert a value using the UCUM format units.
export function UCUMToString(value, unit, defaultValue = undefined, decimalPoints = 1) {
	if (unit in UCUMToFormatter) {
		return UCUMToFormatter[unit](value, decimalPoints);
	}

	const [baseUnitWithPrefix, ...rest] = unit.split("/");
	const postfix = (rest.length ? "/" : "") + rest.join("/");

	// Determine which unit it is.
	for (let nbChars = 1; nbChars <= Math.min(maxUCUMToPresetsLength, baseUnitWithPrefix.length); ++nbChars) {
		const potentialUnit = baseUnitWithPrefix.slice(-nbChars);
		if (potentialUnit in UCUMToPresets) {
			const potentialPrefix = baseUnitWithPrefix.slice(0, -nbChars);
			const preset = UCUMToPresets[potentialUnit];
			if (potentialPrefix in preset.prefixes) {
				return UnitToStringFactory.makeFromPreset(
					value,
					potentialPrefix,
					preset.unit ?? potentialUnit,
					preset,
					decimalPoints,
				);
			}
		}
	}

	return defaultValue;
}

export function frequencyToString(value, decimalPoints = 1) {
	return UnitToStringFactory.makeFromPreset(value, "", "Hz", UCUMToPresets["Hz"], decimalPoints);
}

export function bytesToString(value, decimalPoints = 1) {
	return UnitToStringFactory.makeFromPreset(value, "", "B", UCUMToPresets["By"], decimalPoints);
}

/// Convert a time in seconds into a string.
export function timeToString(value, decimalPoints = 1) {
	return UCUMToFormatter["s"](value, decimalPoints);
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
