import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import TimeseriesCollection from "#bzd/apps/artifacts/plugins/nodes/frontend/timeseries_collection.mjs";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "timeseries-collection");

describe("Timeseries Collection", () => {
	it("insertion", () => {
		const collection = new TimeseriesCollection();

		collection.add({
			a: [
				[1, 10],
				[2, 20],
				[3, 30],
			],
		});
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[2, 20],
			[3, 30],
		]);

		collection.add({ a: [[4, 40]] });
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[2, 20],
			[3, 30],
			[4, 40],
		]);

		collection.add({ a: [[2.5, 25]] });
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[2, 20],
			[2.5, 25],
			[3, 30],
			[4, 40],
		]);

		collection.add({ a: [[2, 20]] });
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[2, 20],
			[2.5, 25],
			[3, 30],
			[4, 40],
		]);

		collection.add({
			a: [
				[2, 20],
				[3, 30],
			],
		});
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[2, 20],
			[3, 30],
			[4, 40],
		]);

		collection.add({
			a: [
				[2, 20],
				[3.5, 35],
			],
		});
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[2, 20],
			[3.5, 35],
			[4, 40],
		]);

		collection.add({
			a: [
				[1.9, 19],
				[2.1, 21],
			],
		});
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[1.9, 19],
			[2.1, 21],
			[3.5, 35],
			[4, 40],
		]);

		collection.add({ a: [[1, 10]] });
		Exception.assertEqual(collection.get("a"), [
			[1, 10],
			[1.9, 19],
			[2.1, 21],
			[3.5, 35],
			[4, 40],
		]);

		collection.add({ a: [[0, 0]] });
		Exception.assertEqual(collection.get("a"), [
			[0, 0],
			[1, 10],
			[1.9, 19],
			[2.1, 21],
			[3.5, 35],
			[4, 40],
		]);

		collection.add({
			a: [
				[0, 0],
				[10, 100],
			],
		});
		Exception.assertEqual(collection.get("a"), [
			[0, 0],
			[10, 100],
		]);
	});

	it("periodRange", () => {
		const collection = new TimeseriesCollection();
		collection.reset({ periodLimit: 1.5 });

		collection.add({
			a: [
				[1, 10],
				[2, 20],
				[3, 30],
			],
		});
		Exception.assertEqual(collection.get("a"), [
			[2, 20],
			[3, 30],
		]);

		collection.add({ a: [[3.5, 35]] });
		Exception.assertEqual(collection.get("a"), [
			[2, 20],
			[3, 30],
			[3.5, 35],
		]);

		collection.add({ a: [[4, 40]] });
		Exception.assertEqual(collection.get("a"), [
			[3, 30],
			[3.5, 35],
			[4, 40],
		]);
	});
});
