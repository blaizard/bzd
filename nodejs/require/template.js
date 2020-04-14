"use strict";

const Fs = require("fs");
const Exception = require("./exception.js")("template");

class Template {
	constructor(template, options) {
		this.options = Object.assign({
			/**
			 * The path of the template used (if relevant)
			 */
			path: ""
		}, options);

		Exception.assert(typeof template === "string", "The template is not a string");
		this.template = template;

		this.pattern = new RegExp("<%(.+?(?=%>))%>", "g");
	}

	/**
	 * Open a template from a path
	 */
	static fromFileSync(path) {
		return new Template(Fs.readFileSync(path).toString(), {
			path: path
		});
	}

	getPath() {
		return this.options.path;
	}

	/**
	 * Return a value. Support nested values "." and arrays "[...]".
	 *
	 * Need to be reworked.
	 */
	getValue(args, key) {
		let index = -1;

		const getNextValueInternal = () => {
			let curArgs = args;

			while (++index < key.length || key[index] == "]")
			{
				let curKey = "";
				for (; index<key.length && (".[]".indexOf(key[index]) === -1); ++index) {
					curKey += key[index];
				}

				Exception.assert(typeof curArgs === "object", () => ("The key \"" + key + "\" is invalid (error with \"" + curKey + "\")."));
				Exception.assert(curArgs.hasOwnProperty(curKey), () => ("The key \"" + curKey + "\" in \"" + JSON.stringify(curArgs) + "\" is not available"));
				curArgs = curArgs[curKey];

				switch (key[index]) {
				case "[":
					const curKey = getNextValueInternal();
					curArgs = curArgs[curKey];
				}
			}
			return curArgs;
		}

		return getNextValueInternal();
	}

	process(args) {

		let output = "";
		let index = 0;
		let m = null;
		do {
			m = this.pattern.exec(this.template);

			// Copy the string that has not been processed and update the current index
			output += this.template.substring(index, ((m) ? m.index : undefined));
			index = (m) ? (m.index + m[0].length) : 0;

			// Handle the operation if any match
			if (m) {
				output += this.getValue(args, (m[1]).trim());
			}
		} while (m);

		return output;
	}
};

module.exports = Template;
