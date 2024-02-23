/// Load a script programmatically.
export default function loadScript(url, type = "text/javascript") {
	return new Promise((resolve, reject) => {
		try {
			const script = document.createElement("script");
			script.type = type;
			script.src = url;
			script.async = true;

			script.addEventListener("load", (ev) => {
				resolve();
			});

			script.addEventListener("error", (ev) => {
				reject("Failed to load the script " + url);
			});

			document.body.appendChild(script);
		} catch (error) {
			reject(error);
		}
	});
}
