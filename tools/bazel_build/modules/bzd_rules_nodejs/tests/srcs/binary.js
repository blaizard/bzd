import a from "./a.js";
import c from "./c.js";
import assert from "node:assert";

assert.equal(a(), "ba");
assert.equal(c(), "dc");
