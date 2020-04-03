"use strict";

const Log = require('../log.js')("test", "template");
const Exception = require('../exception.js')("test", "template");
const Template = require('../template.js');

Log.mute();

describe('Template', () => {
	describe('Simple', () => {
		it('constructor (good)', async () => {
			new Template("This is a test");
		});
		it('constructor (wrong arg)', async () => {
			Exception.assertThrows(() => {
				new Template(4554);
			});
		});
		it('nop', async () => {
			let t = new Template("This is a test");
			Exception.assert(t.process() == "This is a test");
		});
		it('update (one)', async () => {
			let t = new Template("<%hello%>");
			Exception.assert(t.process({hello: "world"}) == "world");
		});
		it('update (one and spaces)', async () => {
			let t = new Template("<%    hello     %>");
			Exception.assert(t.process({hello: "world"}) == "world");
		});
		it('update', async () => {
			let t = new Template("hello <% hello %>");
			Exception.assert(t.process({hello: "world"}) == "hello world");
		});
		it('update (multi)', async () => {
			let t = new Template("<%a%> <%b%>");
			Exception.assert(t.process({a: "hello", b: "world"}) == "hello world");
		});
	});

	describe("Nested", () => {
		it("unique", async () => {
			let t = new Template("<%hello.world%>");
			Exception.assert(t.process({hello: {world: "abc"}}) == "abc");
		});
		it("unique (deep)", async () => {
			let t = new Template("<%a.b.c.d.e.f.g%>");
			Exception.assert(t.process({a: {b: {c: {d: {e: {f: {g: "abc"}}}}}}}) == "abc");
		});
		it("multi", async () => {
			let t = new Template("<% hello.world %><% hello.planet %>");
			Exception.assert(t.process({hello: {world: "abc", planet: "def"}}) == "abcdef");
		});
	});

	describe("Array", () => {
		it("simple", async () => {
			let t = new Template("<%hello[index]%>");
			Exception.assert(t.process({index: "world", hello: {world: "abc"}}) == "abc");
		});
		it("nested 1st element", async () => {
			let t = new Template("<%hello.world[index]%>");
			Exception.assert(t.process({index: "a", hello: {world: {a: "abc"}}}) == "abc");
		});
		it("nested 2nd element", async () => {
			let t = new Template("<%hello[my.index]%>");
			Exception.assert(t.process({my: {index: "a"}, hello: {a: "abc"}}) == "abc");
		});
	});
});
