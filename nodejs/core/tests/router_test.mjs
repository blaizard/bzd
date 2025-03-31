import Router from "../router.mjs";
import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("test", "router");

describe("Router", () => {
	describe("Dispatch", () => {
		const router = new Router();
		let lastPath = null;
		let lastVars = null;
		for (const path of [
			"/hello",
			"/world",
			"/hello/nested",
			"/string/{word}",
			"/int/{a:i}",
			"/star/{args:*}",
			"/plus/{args:+}",
		]) {
			router.add(path, (vars) => {
				lastPath = path;
				lastVars = vars;
			});
		}

		it("/hello", async () => {
			Exception.assert((await router.dispatch("/hello")) !== false);
			Exception.assertEqual(lastPath, "/hello");
			Exception.assertEqual(lastVars, {});
		});

		it("/hello/", async () => {
			Exception.assert((await router.dispatch("/hello/")) === false);
		});

		it("/hello/nested", async () => {
			Exception.assert((await router.dispatch("/hello/nested")) !== false);
			Exception.assertEqual(lastPath, "/hello/nested");
		});

		it("/string/one", async () => {
			Exception.assert((await router.dispatch("/string/one")) !== false);
			Exception.assertEqual(lastPath, "/string/{word}");
			Exception.assertEqual(lastVars, { word: "one" });
		});

		it("/string/one%2Ftwo", async () => {
			Exception.assert((await router.dispatch("/string/one%2Ftwo")) !== false);
			Exception.assertEqual(lastPath, "/string/{word}");
			Exception.assertEqual(lastVars, { word: "one/two" });
		});

		it("/string/one/two", async () => {
			Exception.assert((await router.dispatch("/string/one/two")) === false);
		});

		it("/string/", async () => {
			Exception.assert((await router.dispatch("/string/")) === false);
		});

		it("/int/42", async () => {
			Exception.assert((await router.dispatch("/int/42")) !== false);
			Exception.assertEqual(lastPath, "/int/{a:i}");
			Exception.assertEqual(lastVars, { a: 42 });
		});

		it("/int/", async () => {
			Exception.assert((await router.dispatch("/int/")) === false);
		});

		it("/int/notanint", async () => {
			Exception.assert((await router.dispatch("/int/notanint")) === false);
		});

		it("/int/1/2", async () => {
			Exception.assert((await router.dispatch("/int/1/2")) === false);
		});

		it("/star/one", async () => {
			Exception.assert((await router.dispatch("/star/one")) !== false);
			Exception.assertEqual(lastPath, "/star/{args:*}");
			Exception.assertEqual(lastVars, { args: "one" });
		});

		it("/star/one/two", async () => {
			Exception.assert((await router.dispatch("/star/one/two")) !== false);
			Exception.assertEqual(lastPath, "/star/{args:*}");
			Exception.assertEqual(lastVars, { args: "one/two" });
		});

		it("/star/", async () => {
			Exception.assert((await router.dispatch("/star/")) !== false);
			Exception.assertEqual(lastPath, "/star/{args:*}");
			Exception.assertEqual(lastVars, { args: "" });
		});

		it("/star", async () => {
			Exception.assert((await router.dispatch("/star")) !== false);
			Exception.assertEqual(lastPath, "/star/{args:*}");
			Exception.assertEqual(lastVars, { args: "" });
		});

		it("/star/one%2Ftwo/three", async () => {
			Exception.assert((await router.dispatch("/star/one%2Ftwo/three")) !== false);
			Exception.assertEqual(lastPath, "/star/{args:*}");
			Exception.assertEqual(lastVars, { args: "one%2Ftwo/three" });
		});

		it("/plus/one", async () => {
			Exception.assert((await router.dispatch("/plus/one")) !== false);
			Exception.assertEqual(lastPath, "/plus/{args:+}");
			Exception.assertEqual(lastVars, { args: "one" });
		});

		it("/plus/one/two", async () => {
			Exception.assert((await router.dispatch("/plus/one/two")) !== false);
			Exception.assertEqual(lastPath, "/plus/{args:+}");
			Exception.assertEqual(lastVars, { args: "one/two" });
		});

		it("/plus/", async () => {
			Exception.assert((await router.dispatch("/plus/")) === false);
		});

		it("/plus", async () => {
			Exception.assert((await router.dispatch("/plus")) === false);
		});

		it("/plus/one%2Ftwo/three", async () => {
			Exception.assert((await router.dispatch("/plus/one%2Ftwo/three")) !== false);
			Exception.assertEqual(lastPath, "/plus/{args:+}");
			Exception.assertEqual(lastVars, { args: "one%2Ftwo/three" });
		});
	});

	describe("toCompiledRoute.constructor", () => {
		it("{word}", async () => {
			Router.toCompiledRoute("{word}");
		});
		it("{args:*}", async () => {
			Router.toCompiledRoute("{args:*}");
		});
		it("/hello{args:*}", async () => {
			Exception.assertThrows(() => {
				Router.toCompiledRoute("/hello{args:*}");
			});
		});
		it("/hello{args:+}", async () => {
			Exception.assertThrows(() => {
				Router.toCompiledRoute("/hello{args:+}");
			});
		});
	});

	describe("toCompiledRoute.match", () => {
		it("/hello", async () => {
			const result = Router.toCompiledRoute("/hello").match("/hello");
			Exception.assertEqual(result, {});
		});

		it("/nomatch", async () => {
			const result = Router.toCompiledRoute("/hello").match("/nomatch");
			Exception.assertEqual(result, false);
		});

		it("/int/42", async () => {
			const result = Router.toCompiledRoute("/int/{a:i}").match("/int/42");
			Exception.assertEqual(result, { a: 42 });
		});

		it("/plus/{args:+}", async () => {
			const result = Router.toCompiledRoute("/plus/{args:+}").match("/plus/hello/you");
			Exception.assertEqual(result, { args: "hello/you" });
		});

		it("/plus/", async () => {
			const result = Router.toCompiledRoute("/plus/{args:+}").match("/plus/");
			Exception.assertEqual(result, false);
		});

		it("/star/{args:*}", async () => {
			const result = Router.toCompiledRoute("/star/{args:*}").match("/star/one%2Ftwo/three");
			Exception.assertEqual(result, { args: "one%2Ftwo/three" });
		});

		it("/star/", async () => {
			const result = Router.toCompiledRoute("/star/{args:*}").match("/star/");
			Exception.assertEqual(result, { args: "" });
		});

		it("/star", async () => {
			const result = Router.toCompiledRoute("/star/{args:*}").match("/star");
			Exception.assertEqual(result, { args: "" });
		});
	});

	describe("toCompiledRoute.toRoute", () => {
		it("/hello", async () => {
			const result = Router.toCompiledRoute("/hello").toRoute();
			Exception.assertEqual(result, "/hello");
		});

		it("/int/{a:i}", async () => {
			const result = Router.toCompiledRoute("/int/{a:i}").toRoute({ a: 42 });
			Exception.assertEqual(result, "/int/42");
		});

		it("/int/{a:i}/more", async () => {
			const result = Router.toCompiledRoute("/int/{a:i}/more").toRoute({ a: 42 });
			Exception.assertEqual(result, "/int/42/more");
		});

		it("/plus/{args:+}", async () => {
			const result = Router.toCompiledRoute("/plus/{args:+}").toRoute({ args: "hello/world" });
			Exception.assertEqual(result, "/plus/hello/world");
		});

		it("/star/hello/world", async () => {
			const result = Router.toCompiledRoute("/star/{args:*}").toRoute({ args: "hello/world" });
			Exception.assertEqual(result, "/star/hello/world");
		});

		it("/star/", async () => {
			const result = Router.toCompiledRoute("/star/{args:*}").toRoute({ args: "" });
			Exception.assertEqual(result, "/star/");
		});

		it("/string/one%2Ftwo", async () => {
			const result = Router.toCompiledRoute("/string/{word}").toRoute({ word: "one/two" });
			Exception.assertEqual(result, "/string/one%2Ftwo");
		});
	});
});
