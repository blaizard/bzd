import { Command } from "commander/esm.mjs";
import ElkToSVG from "./elk_to_svg.mjs";
import BdlToElk from "./bdl_to_elk.mjs";
import Fs from "fs";

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option("--output <path>", "Input json file of the architecture.")
	.arguments("<inputs...>", "Inputs in the form of pair, (target, bdl_json_path).")
	.parse(process.argv);

(async () => {
	// Read arguments
	const output = program.opts().output;
	const inputs = program.args.reduce((result, _, index, array) => {
		if (index % 2 === 0) result.push(array.slice(index, index + 2));
		return result;
	}, []);

	for (const [target, bdlPath] of inputs) {
		const content = await Fs.promises.readFile(bdlPath);
		const bdl = JSON.parse(content);

		const converter = new BdlToElk(bdl, target);
		const elk = converter.process(/*onlyApplication*/ true);

		const renderer = new ElkToSVG();
		const svg = await renderer.render(elk);

		console.log(output);
		console.log(svg);

		await Fs.promises.writeFile(output, svg);
	}
})();
