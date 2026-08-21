import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { localCommand, Status } from "#bzd/nodejs/utils/run.js";

declare function describe(description: string, callback: () => void): void;
declare function it(description: string, callback: () => void | Promise<void>): void;

const Exception = ExceptionFactory("test", "utils", "run");

describe("localCommand", () => {
    it("Runs a command and captures its stdout", async () => {
        const result = await localCommand(["echo", "hello"]).join();
        Exception.assertEqual(result.getStdout(), "hello\n");
        Exception.assertEqual(result.isSuccess(), true);
        Exception.assertEqual(result.getReturnCode(), 0);
    });

    it("Captures the final status", async () => {
        const result = await localCommand(["echo", "hello"]).join();
        Exception.assertEqual(result.status, Status.completed);
    });

    it("Fails on a non-zero return code", async () => {
        const result = await localCommand(["sleep", "invalid"], { ignoreFailure: true }).join();
        Exception.assertEqual(result.isFailure(), true);
        Exception.assert(result.getReturnCode() !== 0, "Expected a non-zero return code");
        Exception.assertEqual(result.status, Status.failed);
    });

    it("Rejects by default on failure", async () => {
        await Exception.assertThrows(async () => localCommand(["sleep", "invalid"]).join());
    });

    it("Captures stderr", async () => {
        const result = await localCommand(["sleep", "invalid"], { ignoreFailure: true }).join();
        Exception.assert(result.getStderr().includes("invalid"), "Expected stderr to contain the error message");
    });

    it("Does not buffer output when maxOutputSize is zero", async () => {
        const result = await localCommand(["echo", "hello"], { maxOutputSize: 0 }).join();
        Exception.assertEqual(result.getOutput(), "");
    });

    it("Limits the buffered output size", async () => {
        const result = await localCommand(["echo", "123456789"], { maxOutputSize: 3, ignoreFailure: true }).join();
        Exception.assert(result.getOutput().length <= 3, "Output exceeds max size");
    });

    it("Sends data to the stdout handler", async () => {
        let output = "";
        await localCommand(["echo", "hello"], {
            stdout: (data) => {
                output += data;
            },
        }).join();
        Exception.assertEqual(output, "hello\n");
    });

    it("Triggers status events", async () => {
        const statuses: string[] = [];
        const result = localCommand(["echo", "hello"]);
        result.on("status", (status) => {
            statuses.push(status);
        });
        await result.join();
        Exception.assert(statuses.includes(Status.running), "Missing running status");
        Exception.assert(statuses.includes(Status.completed), "Missing completed status");
    });

    it("Rejects unsupported event topics", () => {
        Exception.assertThrows(() => localCommand(["echo", "hello"]).on("bogus", () => {}));
    });
});

describe("Status", () => {
    it("Defines the expected statuses", () => {
        Exception.assertEqual(Status.idle, "idle");
        Exception.assertEqual(Status.running, "running");
        Exception.assertEqual(Status.failed, "failed");
        Exception.assertEqual(Status.completed, "completed");
        Exception.assertEqual(Status.cancelled, "cancelled");
    });
});