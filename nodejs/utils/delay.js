export async function delayMs(durationMs) {
	return new Promise((resolve) => {
		setTimeout(() => {
			resolve();
		}, durationMs);
	});
}
