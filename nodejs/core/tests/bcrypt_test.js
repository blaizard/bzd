import ExceptionFactory from "../exception.js";
import BCrypt from "../cryptography/bcrypt.js";

const Exception = ExceptionFactory("test", "bcrypt");

describe("BCrypt", () => {
	describe("Simple", () => {
		it("Hash", async () => {
			const hash = await BCrypt.hash("Hello world!");
			Exception.assert(await BCrypt.compare(hash, "Hello world!"));
			Exception.assert(!(await BCrypt.compare(hash, "Hello world!!")));
		}).timeout(10000);
	});
});
