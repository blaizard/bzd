import Fs from "fs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import ExceptionFactory from "./exception.mjs";

const Exception = ExceptionFactory("template");

export default class Template {
	constructor(template, options) {
		this.options = Object.assign(
			{
				/**
				 * The path of the template used (if relevant)
				 */
				path: "",
			},
			options,
		);

		Exception.assert(typeof template === "string", "The template is not a string");
		this.template = template;

		this.pattern = new RegExp("{{(.+?(?=}}))}}", "g");
	}

	/// Open a template from a path
	static async fromFile(path) {
		const data = await FileSystem.readFile(path);
		return new Template(data, {
			path: path,
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

			while (++index < key.length || key[index] == "]") {
				let curKey = "";
				for (; index < key.length && ".[]".indexOf(key[index]) === -1; ++index) {
					curKey += key[index];
				}

				Exception.assert(typeof curArgs === "object", "The key '{}' is invalid (error with '{}').", key, curKey);
				Exception.assert(curKey in curArgs, "The key '{}' in '{:j}' is not available", curKey, curArgs);
				curArgs = curArgs[curKey];

				switch (key[index]) {
					case "[": {
						const curKey = getNextValueInternal();
						curArgs = curArgs[curKey];
						break;
					}
				}
			}
			return curArgs;
		};

		return getNextValueInternal();
	}

	process(args) {
		let output = "";
		let index = 0;
		let m = null;
		do {
			m = this.pattern.exec(this.template);

			// Copy the string that has not been processed and update the current index
			output += this.template.substring(index, m ? m.index : undefined);
			index = m ? m.index + m[0].length : 0;

			// Handle the operation if any match
			if (m) {
				output += this.getValue(args, m[1].trim());
			}
		} while (m);

		return output;
	}
}
