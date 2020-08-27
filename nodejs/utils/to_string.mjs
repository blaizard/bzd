export function bytesToString(value) {
	let size = value;
	let unitIndex = 0;
	const unitList = ["B", "KiB", "MiB", "GiB", "TiB"];
	while (size > 768) {
		size /= 1024;
		unitIndex++;
	}
	return String(Math.round(size * 100) / 100) + unitList[unitIndex];
}
