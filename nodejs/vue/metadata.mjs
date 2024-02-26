import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";
import { reactive, watch } from "vue";

const Exception = ExceptionFactory("metadata");

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

function setCanonical(url) {
	let link = document.querySelector("link[rel='canonical']") || document.createElement("link");
	link.rel = "canonical";
	link.href = url;
	document.getElementsByTagName("head")[0].appendChild(link);
}

function setSitemap(sitemap) {
	let link = document.querySelector("link[rel='sitemap']") || document.createElement("link");
	link.rel = "sitemap";
	link.type = "application/xml";
	link.title = "Sitemap";
	link.href = sitemap;
	document.getElementsByTagName("head")[0].appendChild(link);
}

function getBase() {
	return window.location.protocol + "//" + window.location.host;
}

export default {
	install: (app, values = {}) => {
		const data = reactive({
			icon: "",
			description: "",
			keywords: [],
			title: "",
			base: "",
			route: "",
			sitemap: "",
		});

		watch(
			() => data.icon,
			(value) => {
				setIcon(value);
			},
		);
		watch(
			() => data.description,
			(value) => {
				setMeta("description", value);
			},
		);
		watch(
			() => data.keywords,
			(value) => {
				setMeta("keywords", value.join(", "));
			},
		);
		watch(
			() => data.title,
			(value) => {
				setTitle(value);
			},
		);
		watch(
			() => data.sitemap,
			(value) => {
				setSitemap(value);
			},
		);
		watch(
			() => [data.base, data.route],
			([base, route]) => {
				const value = base.replace(/\/*$/g, "") + "/" + route.replace(/^\/*/g, "");
				setCanonical(value);
			},
		);

		const validation = new Validation({
			icon: "",
			description: "",
			keywords: "",
			title: "",
			base: "",
			route: "",
			sitemap: "",
		});
		validation.validate(values, { all: true });
		Object.assign(
			data,
			{
				base: getBase(),
			},
			values,
		);

		app.config.globalProperties.$metadata = data;
	},
};
