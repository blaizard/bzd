import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Scopes from "#bzd/nodejs/core/authentication/scopes.mjs";

const Exception = ExceptionFactory("test", "scopes");

describe("Scopes", () => {
	describe("contains", () => {
		it("simple", () => {
			const scopes = new Scopes(["/hello"]);
			Exception.assert(scopes.contains("/hello"));
			Exception.assert(!scopes.contains("/"));
			Exception.assert(scopes.contains("/hello/world"));
			Exception.assert(!scopes.contains("/helloworld"));
			Exception.assert(!scopes.contains("/hell"));
			Exception.assert(!scopes.contains(""));
		});
		it("empty", () => {
			const scopes = new Scopes([""]);
			Exception.assert(!scopes.contains("/hello"));
			Exception.assert(!scopes.contains(""));
		});
		it("allow all", () => {
			const scopes = new Scopes(["/"]);
			Exception.assert(scopes.contains("/hello"));
			Exception.assert(scopes.contains("/world/map"));
		});
	});

	describe("matchAny", () => {
		it("simple", () => {
			const scopes = new Scopes(["/hello", "/world"]);
			Exception.assert(scopes.matchAny("/hello"));
			Exception.assert(scopes.matchAny("/world"));
			Exception.assert(!scopes.matchAny("/no"));
			Exception.assert(scopes.matchAny("/hello/world"));
			Exception.assert(scopes.matchAny(["/no", "/hello/world"]));
		});
	});

	describe("sameAs", () => {
		it("simple", () => {
			const scopes = new Scopes(["/hello", "/world"]);
			Exception.assert(!scopes.sameAs(["/hello"]));
			Exception.assert(scopes.sameAs(["/world", "/hello"]));
			Exception.assert(scopes.sameAs(["/world", "/hello", "/hello"]));
			Exception.assert(!scopes.sameAs(["/", "/hello", "/hello"]));
			Exception.assert(!scopes.sameAs([]));
		});
	});

	describe("filterDictionary", () => {
		it("simple", () => {
			const scopes = new Scopes(["/hello", "/world"]);
			Exception.assertEqual(
				scopes.filterDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: "/",
						b: "/hello/world",
						c: "/world",
					},
				),
				{
					b: 2,
					c: 3,
				},
			);
			Exception.assertEqual(
				scopes.filterDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: "/",
						c: "/world",
					},
				),
				{
					c: 3,
				},
			);
			Exception.assertEqual(
				scopes.filterDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: ["/", "/hello"],
						c: "/world",
					},
				),
				{
					a: 1,
					c: 3,
				},
			);
		});
		it("allow all", () => {
			const scopes = new Scopes(["/", "/self/basic/r"]);
			Exception.assertEqual(
				scopes.filterDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: "/self/basic/r",
						b: "/self/basic/w",
						c: "/self/token",
					},
				),
				{
					a: 1,
					b: 2,
					c: 3,
				},
			);
		});
	});

	describe("checkDictionary", () => {
		it("simple", () => {
			const scopes = new Scopes(["/hello", "/world"]);
			Exception.assertResultError(
				scopes.checkDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: "/",
						c: "/world",
					},
				),
			);
			Exception.assertResult(
				scopes.checkDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: "/hello",
						b: "/hello/this",
						c: "/world",
					},
				),
			);
			Exception.assertResultError(
				scopes.checkDictionary(
					{
						a: 1,
						b: 2,
						c: 3,
					},
					{
						a: "/hello",
						c: "/world",
					},
				),
			);
			Exception.assertResult(
				scopes.checkDictionary(
					{
						a: 1,
						c: 3,
					},
					{
						a: "/hello",
						b: "/",
						c: "/world",
					},
				),
			);
		});
	});

	describe("checkValid", () => {
		it("simple", () => {
			const scopes = new Scopes(["/hello", "/world"]);
			Exception.assertResult(scopes.checkValid(["/hello"]));
			Exception.assertResult(scopes.checkValid(["/hello", "/world", "/hello/you"]));
			Exception.assertResultError(scopes.checkValid(["/hello", "/world", "/"]));
		});
	});
});
