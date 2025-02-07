const loadScript = async function (url, type = "text/javascript") {
	return new Promise((resolve, reject) => {
		try {
			const id = "loadScript_" + url.replace(/[^a-zA-Z0-9]+/g, "_");
			if (document.getElementById(id)) {
				resolve();
				return;
			}

			const script = document.createElement("script");
			script.type = type;
			script.id = id;
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
};
