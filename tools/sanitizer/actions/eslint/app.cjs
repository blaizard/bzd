const { ESLint } = require("eslint");

(async () => {
	const workspace = process.argv[2];

	// 1. Create an instance with the `fix` option.
	const eslint = new ESLint({
		fix: true,
		cwd: workspace,
		resolvePluginsRelativeTo: process.cwd(),
		overrideConfigFile: process.cwd() + "/tools/sanitizer/actions/eslint/.eslintrc.json"
	});

	// 2. Lint files. This doesn't modify target files.
	const results = await eslint.lintFiles(["**/*.{vue,js,mjs,cjs}"]);

	// 3. Modify the files with the fixed code.
	await ESLint.outputFixes(results);

	// 4. Format the results.
	const formatter = await eslint.loadFormatter("stylish");
	const resultText = formatter.format(results);

	// 5. Output it.
	console.log(resultText);

	// Return an error if the error count is > 0
	const totalErrorCount = results.reduce((total, item) => {
		return total + (item.errorCount - item.fixableErrorCount);
	}, 0);
	if (totalErrorCount > 0) {
		process.exitCode = 2;
	}
})().catch((error) => {
	process.exitCode = 1;
	console.error(error);
});
