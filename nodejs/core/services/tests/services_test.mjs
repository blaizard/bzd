import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import ServiceProvider from "#bzd/nodejs/core/services/provider.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";

const Exception = ExceptionFactory("test", "services");

async function waitFor(callback, timeoutS = 5) {
	return new Promise((resolve, reject) => {
		let instances = {
			valid: null,
			invalid: null,
		};
		instances.valid = setInterval(() => {
			if (callback()) {
				clearTimeout(instances.valid);
				clearTimeout(instances.invalid);
				resolve();
			}
		}, 1);
		instances.invalid = setTimeout(() => {
			clearTimeout(instances.valid);
			reject("Timeout");
		}, timeoutS * 1000);
	});
}

async function* asyncGeneratorTest() {
	let counter = 42;
	while (true) {
		yield counter;
		++counter;
	}
}

describe("Services", () => {
	let startCounter = 0;
	let stopCounter = 0;
	let timeTriggeredCounter1 = 0;
	let timeTriggeredCounter2 = 0;

	const services = new Services();

	describe("Provider", () => {
		const provider = new ServiceProvider("test", "services");

		it("Initial state", () => {
			Exception.assertEqual([...provider.getStartProcesses()].length, 0);
			Exception.assertEqual([...provider.getStopProcesses()].length, 0);
			Exception.assertEqual([...provider.getTimeTriggeredProcesses()].length, 0);
		});

		it("Start process", () => {
			provider.addStartProcess(() => ++startCounter);

			const processes = [...provider.getStartProcesses()];
			Exception.assertEqual(processes.length, 1);
			Exception.assertEqual(processes[0][0], "start.1");
			Exception.assert("options" in processes[0][1]);
		});

		it("Stop process", () => {
			provider.addStopProcess(() => ++stopCounter);

			const processes = [...provider.getStopProcesses()];
			Exception.assertEqual(processes.length, 1);
			Exception.assertEqual(processes[0][0], "stop.1");
			Exception.assert("options" in processes[0][1]);
		});

		it("Time triggered process", () => {
			provider.addTimeTriggeredProcess("1", () => ++timeTriggeredCounter1, {
				periodS: 0.1,
				delayS: 0,
			});
			provider.addTimeTriggeredProcess(
				"2",
				() => {
					++timeTriggeredCounter2;
					throw "noooo";
				},
				{
					periodS: 0.2,
					delayS: 0,
				},
			);
			provider.addTimeTriggeredProcess("3", asyncGeneratorTest(), {
				periodS: 0.2,
				delayS: 0,
			});

			const processes = [...provider.getTimeTriggeredProcesses()];
			Exception.assertEqual(processes.length, 3);
			Exception.assertEqual(processes[0][0], "1");
			Exception.assertEqual(processes[1][0], "2");
			Exception.assertEqual(processes[2][0], "3");
		});

		it("register", () => {
			services.register(provider);
		});
	});

	describe("Service", () => {
		let uid = null;

		it("query", async () => {
			const query = [...services.getServices()];
			Exception.assertEqual(query.length, 1);
			const [uid0, state] = query[0];
			Exception.assertEqual(state.status, Services.Status.idle);
			uid = uid0;
		});

		it("start", async () => {
			await services.start();
			const state = services.getService(uid);
			Exception.assertEqual(state.status, Services.Status.running);
			Exception.assertEqual(startCounter, 1);
			Exception.assertEqual(stopCounter, 0);
		});

		it("wait for execution", async () => {
			await waitFor(() => {
				const record3 = services.getProcess(uid, "3");
				if (record3.executions < 2) {
					return false;
				}
				return timeTriggeredCounter1 > 1 && timeTriggeredCounter2 > 0;
			});
			const record1 = services.getProcess(uid, "1");
			Exception.assert(record1.executions >= 1);
			Exception.assertEqual(record1.errors, 0);

			const record2 = services.getProcess(uid, "2");
			Exception.assert(record2.executions >= 1);
			Exception.assert(record2.errors >= 1);

			const record3 = services.getProcess(uid, "3");
			Exception.assert(record3.executions >= 2);
			Exception.assertEqual(record3.errors, 0);
			Exception.assertEqual(record3.logs[1].result, 42);
			Exception.assertEqual(record3.logs[0].result, 43);
		});

		it("stop", async () => {
			await services.stop();
			Exception.assertEqual(startCounter, 1);
			Exception.assertEqual(stopCounter, 1);
		});
	});
});
