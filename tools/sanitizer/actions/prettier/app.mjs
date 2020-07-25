import Prettier from "prettier";
import Workspace from "../../utils/nodejs/workspace.mjs";
import FileSystem from "bzd/core/filesystem.mjs";

async function evaluateFiles(parser, workspace, options) {
	const files = new Workspace(workspace, options);

	for await (const path of files.data()) {
		const content = await FileSystem.readFile(path);
		const formattedContent = Prettier.format(content, {
			printWidth: 120,
			parser: parser,
			useTabs: true,
			singleQuote: false,
			semi: true,
			bracketSpacing: true,
			arrowParens: "always",
			vueIndentScriptAndStyle: true,
		});
		await FileSystem.writeFile(path, formattedContent);
	}
}

(async () => {
	const workspace = process.argv[2];

	let promiseList = [];
	promiseList.push(
		evaluateFiles("babel", workspace, {
			include: ["**.mjs", "**.cjs", "**.js"],
		})
	);
	promiseList.push(
		evaluateFiles("vue", workspace, {
			include: ["**.vue"],
		})
	);
	promiseList.push(
		evaluateFiles("css", workspace, {
			include: ["**.css"],
		})
	);
	promiseList.push(
		evaluateFiles("scss", workspace, {
			include: ["**.scss"],
		})
	);
	promiseList.push(
		evaluateFiles("markdown", workspace, {
			include: ["**.md"],
			exclude: ["docs/api/**"],
		})
	);

	await Promise.all(promiseList);
})().catch((error) => {
	process.exitCode = 1;
	console.error(error);
});
