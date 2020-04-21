"use strict";

import Assert from "assert";

describe("NodeJs", () => {
	it("Version", () => {
		Assert.equal(process.version, "v13.9.0"); 
	});
});
