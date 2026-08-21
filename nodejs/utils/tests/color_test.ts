import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Color from "#bzd/nodejs/utils/color.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "color");

describe("Color", () => {
    describe("toString", () => {
        it("Hex long form", () => {
            Exception.assertEqual(new Color("#ff0000").toString(), "rgba(255, 0, 0, 1)");
        });
        it("Hex short form", () => {
            Exception.assertEqual(new Color("#0f0").toString(), "rgba(0, 255, 0, 1)");
        });
        it("Unsupported format throws", async () => {
            await Exception.assertThrowsWithMatch(() => new Color("bogus"), "Unsupported color format");
        });
    });
    describe("setAlpha", () => {
        it("Alpha channel is updated", () => {
            const color = new Color("#ffffff");
            color.setAlpha(0.5);
            Exception.assertEqual(color.toString(), "rgba(255, 255, 255, 0.5)");
        });
    });
});