"use strict";

import ExceptionFactory from "../exception.mjs";
import Cache from "../cache.mjs";

const Exception = ExceptionFactory("test", "cache");

describe("Cache", () => {
	describe("Simple", () => {
		it("Base", async () => {
            let cache = new Cache({
                garbageCollector: false
            });
            let argRead = 0;
            cache.register("test", (arg) => {
                if (!arg) {
                    throw new Error("BOOM!");
                }
                argRead = arg;
                return "return." + arg;
            });
            const result1 = await cache.get("test", "hello");
            Exception.assertEqual(argRead, "hello");
            Exception.assertEqual(result1, "return.hello");
            const result2 = await cache.get("test", "world");
            Exception.assertEqual(argRead, "world");
            Exception.assertEqual(result2, "return.world");
            await Exception.assertThrows(async () => { await cache.get("tefst", "world"); });
            await Exception.assertThrows(async () => { await cache.get("test"); });
            await Exception.assertThrows(async () => { await cache.get("tedst"); });
            await Exception.assertThrows(async () => { await cache.get(); });
		});
	});
});
