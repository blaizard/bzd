import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";

const Exception = ExceptionFactory("meta");

function setTitle(title) {
	let element = document.querySelector("title") || document.createElement("title");
	element.innerHTML = title;
	document.getElementsByTagName("head")[0].appendChild(element);
}

function setIcon(icon) {
	let link = document.querySelector("link[rel*='icon']") || document.createElement("link");
	link.rel = "icon";
	link.type = "image/x-icon";
	link.href = icon;
	document.getElementsByTagName("head")[0].appendChild(link);
}

function setMeta(name, content) {
	let meta = document.querySelector("meta[name='" + name + "']") || document.createElement("meta");
	meta.name = name;
	meta.content = content;
	document.getElementsByTagName("head")[0].appendChild(meta);
}

export default {
	install: (app, meta = {}) => {
		const validation = new Validation({
			icon: "",
			description: "",
			keywords: "",
			title: "",
		});
		validation.validate(meta, { all: true });

		if ("icon" in meta) {
			setIcon(meta.icon);
		}
		if ("description" in meta) {
			setMeta("description", meta.description);
		}
		if ("keywords" in meta) {
			setMeta("keywords", meta.keywords.join(", "));
		}
		if ("title" in meta) {
			setTitle(meta.title);
		}
	},
};
