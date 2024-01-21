import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("email");

/**
 * Email module
 */
export default class Email {
	constructor() {}

	/**
	 * Send an email
	 */
	async send(to, subject, data) {
		const toList = Array.isArray(to) ? to : [to];
		Exception.assert(typeof data == "object", "'data' must be an object: {}", data);

		const dataUpdated = Object.assign(
			{
				html: null,
				text: null,
			},
			data,
		);

		await this._sendImpl(toList, subject, dataUpdated);
	}
}
