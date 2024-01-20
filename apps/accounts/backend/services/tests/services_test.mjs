import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import ServiceProvider from "#bzd/apps/accounts/backend/services/provider.mjs";
import Services from "#bzd/apps/accounts/backend/services/services.mjs";

const Exception = ExceptionFactory("test", "services");

describe("Services", () => {
	let startCounter = 0;
	let stopCounter = 0;
	let timeTriggeredCounter1 = 0;
	let timeTriggeredCounter2 = 0;

	const services = new Services();

	describe("Provider", () => {
		const provider = new ServiceProvider("test", "services");

		it("Inital state", () => {
			Exception.assertEqual([...provider.getStartProcesses()].length, 0);
			Exception.assertEqual([...provider.getStopProcesses()].length, 0);
			Exception.assertEqual([...provider.getTimeTriggeredProcesses()].length, 0);
		});

		it("Start process", () => {
			provider.addStartProcess(() => ++startCounter);

			const processes = [...provider.getStartProcesses()];
			Exception.assertEqual(processes.length, 1);
			Exception.assertEqual(processes[0][0], "start");
			Exception.assert("options" in processes[0][1]);
		});

		it("Stop process", () => {
			provider.addStopProcess(() => ++stopCounter);

			const processes = [...provider.getStopProcesses()];
			Exception.assertEqual(processes.length, 1);
			Exception.assertEqual(processes[0][0], "stop");
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

			const processes = [...provider.getTimeTriggeredProcesses()];
			Exception.assertEqual(processes.length, 2);
			Exception.assertEqual(processes[0][0], "1");
			Exception.assertEqual(processes[1][0], "2");
		});

		it("register", () => {
			services.register("myservice", provider);
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
		});

		//it("wait for execution", async () => {
		//    await services.start();
		//});
	});
});
