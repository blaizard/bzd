import ExceptionFactory from "../exception.mjs";
import BCrypt from "../cryptography/bcrypt.mjs";

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
