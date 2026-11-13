import { Jimp } from "jimp";
import { thumbHashToRGBA } from "thumbhash";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { fromChunk } from "#bzd/nodejs/core/stream.js";
import Handlers from "#bzd/nodejs/db/data/handlers/handlers.js";
import ImageHandler from "#bzd/apps/artifacts/plugins/nodes/handlers/image.js";
import { File } from "#bzd/apps/artifacts/plugins/nodes/files.js";

const Exception = ExceptionFactory("test", "apps", "plugin", "nodes", "handlers", "image");

const createImage = async (width, height, mime = "image/png") => {
	const image = new Jimp({ width, height, color: 0xff0000ff });
	return await image.getBuffer(mime);
};

const processFile = async (file, options = {}) => {
	const handlers = new Handlers(
		{ "/image": { image: options } },
		{},
		{
			image: (handlerOptions) => new ImageHandler(handlerOptions ?? {}),
		},
	);
	const result = await handlers.process([[["image"], file]]);
	return result[0][1];
};

const assertDimensions = async (file, width, height) => {
	const image = await Jimp.fromBuffer(file.stream);
	Exception.assertEqual([image.width, image.height], [width, height]);
};

const assertThumbHash = (metadata) => {
	Exception.assert(typeof metadata.thumbhash == "string", "Missing thumbhash metadata.");
	Exception.assert(metadata.thumbhash.length > 0, "Empty thumbhash metadata.");
	const image = thumbHashToRGBA(Buffer.from(metadata.thumbhash, "base64"));
	Exception.assert(image.w > 0 && image.h > 0, "Invalid thumbhash metadata.");
};

const rawData = (width, height, channels, stride = width * channels) => {
	const data = Buffer.alloc(stride * height);
	for (let y = 0; y < height; ++y) {
		for (let x = 0; x < width; ++x) {
			for (let channel = 0; channel < channels; ++channel) {
				data[y * stride + x * channels + channel] = (x * 17 + y * 31 + channel * 53) % 256;
			}
		}
	}
	return data;
};

describe("ImageHandler", () => {
	it("converts a png to jpeg and downscales it", async () => {
		const png = await createImage(200, 100, "image/png");
		const output = await processFile(new File({ stream: png, mime: "image/png" }), {
			maxWidth: 64,
			maxHeight: 64,
		});
		Exception.assert(output instanceof File);
		Exception.assertEqual(output.mime, "image/jpeg");
		Exception.assertEqual(output.metadata.type, "image/jpeg");
		Exception.assertEqual([output.metadata.width, output.metadata.height], [64, 32]);
		assertThumbHash(output.metadata);
		await assertDimensions(output, 64, 32);
	});

	it("keeps a jpeg within the bounds unchanged", async () => {
		const jpeg = await createImage(32, 16, "image/jpeg");
		const output = await processFile(
			new File({ stream: jpeg, mime: "application/octet-stream", metadata: { type: "image/jpeg" } }),
			{
				maxWidth: 64,
				maxHeight: 64,
			},
		);
		Exception.assertEqual(output.stream, jpeg);
		Exception.assertEqual(output.mime, "image/jpeg");
		Exception.assertEqual(output.metadata.type, "image/jpeg");
		Exception.assertEqual([output.metadata.width, output.metadata.height], [32, 16]);
		assertThumbHash(output.metadata);
	});

	it("materializes a passthrough jpeg stream", async () => {
		const jpeg = await createImage(32, 16, "image/jpeg");
		const output = await processFile(new File({ stream: fromChunk(jpeg), mime: "image/jpeg" }), {
			maxWidth: 64,
			maxHeight: 64,
		});
		Exception.assertEqual(output.stream, jpeg);
	});

	it("downscales a jpeg exceeding the bounds", async () => {
		const jpeg = await createImage(200, 100, "image/jpeg");
		const output = await processFile(new File({ stream: jpeg, mime: "image/jpeg" }), {
			maxWidth: 64,
			maxHeight: 64,
		});
		Exception.assertEqual(output.mime, "image/jpeg");
		await assertDimensions(output, 64, 32);
	});

	it("preserves the aspect ratio", async () => {
		const png = await createImage(100, 300, "image/png");
		const output = await processFile(new File({ stream: png, mime: "image/png" }), {
			maxWidth: 200,
			maxHeight: 200,
		});
		await assertDimensions(output, 67, 200);
	});

	const rawCases = [
		["raw-rgb", 3, 4],
		["raw-bgr", 3, 4],
		["raw-rgba", 4, 5],
		["raw-gray8", 1, 2],
	];
	for (const [type, channels, padding] of rawCases) {
		it(`converts ${type} with stride`, async () => {
			const width = 4;
			const height = 2;
			const stride = width * channels + padding;
			const output = await processFile(
				new File({
					stream: rawData(width, height, channels, stride),
					mime: "application/octet-stream",
					metadata: {
						type: type,
						width: String(width),
						height: String(height),
						stride: String(stride),
					},
				}),
				{ maxWidth: 64, maxHeight: 64 },
			);
			Exception.assertEqual(output.mime, "image/jpeg");
			Exception.assertEqual(output.metadata.type, "image/jpeg");
			Exception.assertEqual([output.metadata.width, output.metadata.height], [width, height]);
			Exception.assertEqual(output.metadata.stride, undefined);
			assertThumbHash(output.metadata);
			await assertDimensions(output, width, height);
		});
	}

	it("uses the default stride when none is provided", async () => {
		const width = 3;
		const height = 2;
		const output = await processFile(
			new File({
				stream: rawData(width, height, 3),
				mime: "application/octet-stream",
				metadata: {
					type: "raw-rgb",
					width: String(width),
					height: String(height),
				},
			}),
			{ maxWidth: 64, maxHeight: 64 },
		);
		await assertDimensions(output, width, height);
	});

	it("requires dimensions for raw images", async () => {
		await Exception.assertThrowsWithMatch(async () => {
			await processFile(
				new File({
					stream: rawData(4, 2, 3),
					mime: "application/octet-stream",
					metadata: {
						type: "raw-rgb",
						height: "2",
					},
				}),
			);
		}, "width");
	});

	it("does not fall back to magic bytes", async () => {
		const png = await createImage(4, 4, "image/png");
		await Exception.assertThrowsWithMatch(async () => {
			await processFile(new File({ stream: png, mime: "application/octet-stream" }));
		}, "Unsupported image type");
	});

	it("ignores values that are not files", async () => {
		const png = await createImage(4, 4, "image/png");
		const output = await processFile(png);
		Exception.assertEqual(output, png);
	});
});
