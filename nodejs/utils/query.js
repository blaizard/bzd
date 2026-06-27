export function getQueryAsDict() {
	return window.location.search
		.replace("?", "")
		.split("&")
		.filter(Boolean)
		.reduce((object, entry) => {
			const [key, value] = entry.split("=");
			object[decodeURIComponent(key)] = decodeURIComponent(value);
			return object;
		}, {});
}
