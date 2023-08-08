import ExceptionFactory from "../exception.mjs";
import Format from "../format.mjs";

const Exception = ExceptionFactory("test", "format");

describe("Format", () => {
  describe("Simple", () => {
	it("No substitution", () => {
	  const text = Format("Hello world!");
	  Exception.assertEqual(text, "Hello world!");
	});
	it("Single substitution", () => {
	  let text = Format("Hello {}!", "you");
	  Exception.assertEqual(text, "Hello you!");
	});
  });

  describe("Numbered index", () => {
	it("Right order", () => {
	  let text = Format("{0} {1}!", "Hello", "World");
	  Exception.assertEqual(text, "Hello World!");
	});
	it("Inverted", () => {
	  let text = Format("{1} {0}!", "Hello", "World");
	  Exception.assertEqual(text, "World Hello!");
	});
  });

  describe("String index", () => {
	it("Simple", () => {
	  let text = Format("{hello} {world}!", {
		hello : "Hello",
		world : "World",
	  });
	  Exception.assertEqual(text, "Hello World!");
	});
  });
});
