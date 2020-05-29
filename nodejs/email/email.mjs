"use strict";

import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("email");

/**
 * Email module
 */
export default class Email {

	constructor() {
	}

	/**
     * Send an email
     */
	async send(to, subject, data) {
		const toList = (Array.isArray(to)) ? to : [to];
		Exception.assert(typeof data == "object", "'data' must be an object: {}", data);

		const dataUpdated = Object.assign({
			// Content format
			format: "text",
			// Content to be displayed
			content: null
		}, data);

		Exception.assert(["text", "html"].includes(dataUpdated.format), "Only 'text' and 'html' are valid formats: {}", dataUpdated.format);
		Exception.assert(typeof dataUpdated.content == "string", "'content' must be a string: {}", dataUpdated.content);

		await this._sendImpl(toList, subject, dataUpdated);
	}
}
