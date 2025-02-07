import elkToSVG from "./elk_to_svg/entry_point.mjs";
import BdlToElk from "./bdl_to_elk.mjs";

export default async function bdlToSvg(bdls, all) {
	const converter = new BdlToElk();

	let targets = new Set();
	for (const bdl of bdls) {
		converter.addTarget(bdl);
		targets.add(bdl.target);
	}

	const elk = converter.process({
		filterOut: all ? [] : targets,
	});

	const renderer = elkToSVG();
	return await renderer.render(elk);
}
