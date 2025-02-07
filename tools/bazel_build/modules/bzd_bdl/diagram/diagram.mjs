import { Command } from "commander/esm.mjs";
import elkToSVG from "./elk_to_svg/entry_point.mjs";
import BdlToElk from "./bdl_to_elk.mjs";
import Fs from "fs";

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("--output <path>", "Input json file of the architecture.")
	.arguments("<inputs...>", "Inputs of bdl json files, bdl_json_path.")
	.parse(process.argv);

(async () => {
	// Read arguments
	const output = program.opts().output;
	const inputs = program.args;

	const converter = new BdlToElk();

	let targets = new Set();
	for (const bdlPath of inputs) {
		const content = await Fs.promises.readFile(bdlPath);
		const bdl = JSON.parse(content);
		converter.addTarget(bdl);
		targets.add(bdl.target);
	}

	const elk = converter.process({
		filterOut: targets,
	});

	const renderer = elkToSVG();
	const svg = await renderer.render(elk);

	await Fs.promises.writeFile(output, svg);
})();
