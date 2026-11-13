import { Jimp } from "jimp";
import { rgbaToThumbHash } from "thumbhash";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { toBuffer } from "#bzd/nodejs/core/stream.js";
import { File } from "#bzd/apps/artifacts/plugins/nodes/files.js";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "handlers", "image");

/// Resize and encode uploaded images as JPEG.
///
/// The image type is inferred from the `type` metadata (set through a
/// `bzd-metadata-type` header) or the file MIME type. Raw pixel formats require
/// `width`, `height` and optionally `stride` metadata.
export default class ImageHandler {
	constructor({ maxWidth = 1024, maxHeight = 1024, quality = 85 } = {}) {
		this.maxWidth_ = maxWidth;
		this.maxHeight_ = maxHeight;
		this.quality_ = quality;
	}

	async process(fragments) {
		for (const fragment of fragments.all()) {
			if (!(fragment.value instanceof File)) {
				continue;
			}
			await this.processFile_(fragment.value);
		}
	}

	async processFile_(file) {
		const metadata = file.metadata ?? {};
		const type = metadata.type || file.mime;
		const format = ImageHandler.getFormat_(type);
		Exception.assertPrecondition(format, "Unsupported image type: {:?}.", type);

		const bytes = await ImageHandler.getBytes_(file);
		const image = format.channels ? this.fromRaw_(bytes, format, metadata) : await this.decode_(bytes, type);

		const scale = Math.min(1, this.maxWidth_ / image.width, this.maxHeight_ / image.height);
		const width = Math.max(1, Math.round(image.width * scale));
		const height = Math.max(1, Math.round(image.height * scale));
		const needsResize = width !== image.width || height !== image.height;

		if (format.jpeg && !needsResize) {
			file.stream = bytes;
		} else {
			if (needsResize) {
				image.resize({ w: width, h: height });
			}
			if (image.hasAlpha()) {
				image.background = 0xffffffff;
			}
			file.stream = await image.getBuffer("image/jpeg", { quality: this.quality_ });
		}

		file.mime = "image/jpeg";
		this.updateMetadata_(file, image);
	}

	updateMetadata_(file, image) {
		const width = image.width;
		const height = image.height;
		const thumbhash = ImageHandler.toThumbHash_(image);
		Object.assign((file.metadata ??= {}), {
			type: file.mime,
			width: width,
			height: height,
			thumbhash: thumbhash,
		});
		delete file.metadata.stride;
	}

	static toThumbHash_(image) {
		let thumbnail = image;
		if (image.width > 100 || image.height > 100) {
			const scale = Math.min(100 / image.width, 100 / image.height);
			thumbnail = image.clone().resize({
				w: Math.max(1, Math.round(image.width * scale)),
				h: Math.max(1, Math.round(image.height * scale)),
			});
		}
		const hash = rgbaToThumbHash(thumbnail.width, thumbnail.height, thumbnail.bitmap.data);
		return Buffer.from(hash).toString("base64");
	}

	static getFormat_(type) {
		switch (type?.split(";")[0].trim().toLowerCase()) {
			case "image/jpeg":
				return { jpeg: true };
			case "image/png":
			case "image/gif":
			case "image/bmp":
			case "image/x-ms-bmp":
			case "image/tiff":
				return {};
			case "raw-rgb":
				return { channels: 3, order: [0, 1, 2] };
			case "raw-bgr":
				return { channels: 3, order: [2, 1, 0] };
			case "raw-rgba":
				return { channels: 4, order: [0, 1, 2, 3] };
			case "raw-gray8":
				return { channels: 1, order: [0], gray: true };
			default:
				return null;
		}
	}

	static async getBytes_(file) {
		if (Buffer.isBuffer(file.stream)) {
			return file.stream;
		}
		if (file.stream instanceof Uint8Array) {
			return Buffer.from(file.stream.buffer, file.stream.byteOffset, file.stream.byteLength);
		}
		Exception.assertPrecondition(
			file.stream && typeof file.stream.on == "function",
			"The file stream is not readable.",
		);
		return await toBuffer(file.stream);
	}

	fromRaw_(bytes, format, metadata) {
		const width = Number(metadata.width);
		const height = Number(metadata.height);
		Exception.assertPrecondition(
			Number.isInteger(width) && width > 0,
			"Raw image 'width' metadata is required and must be a positive integer, got: {:?}.",
			metadata.width,
		);
		Exception.assertPrecondition(
			Number.isInteger(height) && height > 0,
			"Raw image 'height' metadata is required and must be a positive integer, got: {:?}.",
			metadata.height,
		);

		const rowBytes = width * format.channels;
		const stride =
			metadata.stride === undefined || metadata.stride === null || metadata.stride === ""
				? rowBytes
				: Number(metadata.stride);
		Exception.assertPrecondition(
			Number.isInteger(stride) && stride >= rowBytes,
			"Raw image 'stride' must be an integer greater than or equal to {}, got: {:?}.",
			rowBytes,
			metadata.stride,
		);

		const requiredBytes = stride * (height - 1) + rowBytes;
		Exception.assertPrecondition(
			bytes.length >= requiredBytes,
			"Raw image is too small: {} bytes, expected at least {}.",
			bytes.length,
			requiredBytes,
		);

		const data = Buffer.alloc(width * height * 4);
		for (let y = 0; y < height; ++y) {
			const sourceRow = y * stride;
			const destinationRow = y * width * 4;
			for (let x = 0; x < width; ++x) {
				const sourceIndex = sourceRow + x * format.channels;
				const destinationIndex = destinationRow + x * 4;
				if (format.gray) {
					const value = bytes[sourceIndex];
					data[destinationIndex] = value;
					data[destinationIndex + 1] = value;
					data[destinationIndex + 2] = value;
					data[destinationIndex + 3] = 255;
				} else {
					for (let channel = 0; channel < format.channels; ++channel) {
						data[destinationIndex + channel] = bytes[sourceIndex + format.order[channel]];
					}
					if (format.channels < 4) {
						data[destinationIndex + 3] = 255;
					}
				}
			}
		}

		return Jimp.fromBitmap({ data, width, height });
	}

	async decode_(bytes, type) {
		try {
			return await Jimp.fromBuffer(bytes);
		} catch (e) {
			Exception.errorPrecondition("Failed to decode image of type '{:?}': {}", type, e.message);
		}
	}
}
