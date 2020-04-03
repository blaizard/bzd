"use strict"

const Fs = require("fs");
const Path = require("path");
const { VueLoaderPlugin } = require("vue-loader")
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const WebpackAssetsManifest = require("webpack-assets-manifest");
const BundleAnalyzerPlugin = require("webpack-bundle-analyzer").BundleAnalyzerPlugin;
const Template = require("./require/template.js");
const Log = require("./require/log.js")("webpack");
const Exception = require("./require/exception.js")("webpack");

class Webpack {
	/**
	 * Helper function to deep merge objects
	 */
	static merge(obj1, obj2) {
		let obj = Object.assign({}, obj1);
		for (const key in obj2) {
			if (obj.hasOwnProperty(key) && obj[key] instanceof Array && obj2[key] instanceof Array) {
				obj[key] = obj[key].concat(obj2[key]);
			}
			else if (obj.hasOwnProperty(key) && typeof obj[key] == "object" && typeof obj2[key] == "object") {
				obj[key] = Webpack.merge(obj[key], obj2[key]);
			}
			else if (obj2[key] !== undefined) {
				obj[key] = obj2[key];
			}
		}
		return obj;
	}
	/**
	 * Generate a hash based on a path
	 */
	static hash(path) {
		// Keep 8 characters from the path
		let hashStr = Math.abs(path.split("").reduce((prevHash, currVal) => (((prevHash << 5) - prevHash) + currVal.charCodeAt(0)) | 0, 0)).toString();
		for (let i = 0; i < path.length; i += (path.length / 8)) {
			hashStr += path.charAt(Math.floor(i));
		}
		return Buffer.from(hashStr).toString("base64") + "." + Path.basename(path).split(".").slice(0, -1).join(".");
	}
	/**
	 * Create a directory recursively, if it already exists, do nothing
	 */
	static mkdirSync(path) {
		let curPath = Path.resolve(path);
		let dirList = [];

		// Identify the directories that do not exists
		while (!(Fs.existsSync(curPath))) {
			dirList.unshift(Path.basename(curPath));
			curPath = Path.dirname(curPath);
		}

		// Create them
		while (dirList.length) {
			curPath = Path.join(curPath, dirList.shift());
			Fs.mkdirSync(curPath);
		}
	}
	/**
	 * Generate the webpack configuration
	 */
	static generate(config) {
		config = Webpack.merge({
			/**
			 * Entry points for the application. It uses the following format:
			 * - key: The path of the javascript file
			 * - value: either a string or an array of string of the html files to be generated.
			 *          It also accepts an entry of type Template, that will be processed to generate the file.
			 */
			entries: {},
			/**
			 * Assets to add regardless
			 */
			assets: {
				css: [],
				js: []
			},
			/**
			 * Output path of the generated content.
			 * If unset, this option must be defined with the CLI via --output-path
			 */
			output: null,
			/**
			 * The url where the resource should be fetched by the client
			 */
			publicPath: "",
			/**
			 * Directory path where to store temporary files
			 */
			tempPath: Path.join(__dirname, ".temp"),
			/**
			 * Set the output type. It can be "html", "node" or "library".
			 */
			type: "html",
			/**
			 * Path aliases
			 */
			alias: {
				"[lib]": __dirname
			},
			/**
			 * List of templates to be generated
			 */
			templates: [],
			/**
			 * Hooks to interact with the generation process
			 */
			hooks: {
				/**
				 * Write a manifest.json that is used to 
				 */
				manifest: (manifest, config) => manifest,
				/**
				 * Called at the end, after the manifest has been written to disk
				 */
				end: (manifest, config) => {}
			},
			/**
			 * Add custom loaders. For example:
			 * { irhtml: "./irhtml-loader.js" }
			 */
			loaders: {},
			/**
			 * User defined arguments that can be used as data for the template for example
			 */
			args: {},
			// ---- For private use only --------------------------------------
			/**
			 * List of temporary files associated with this config object
			 */
			temp: []
		}, config);

		// ---- Sanity check --------------------------------------------------
		Exception.assert(Object.keys(config.entries).length > 0, "No entries are defined");

		// ---- Register events to clean the temporary files on exit ----------
		["exit", "SIGINT", "SIGUSR1", "SIGUSR2", "uncaughtException", "SIGTERM"].forEach((eventType) => {
			process.on(eventType, cleanUp.bind(null, config));
		});

		// ---- Execute template for entries ----------------------------------
		for (const entryId in config.entries) {
			config.entries[entryId] = (config.entries[entryId] instanceof Array) ? config.entries[entryId] : [config.entries[entryId]];
			config.entries[entryId] = config.entries[entryId].map((entry) => {
				if (entry instanceof Template) {
					Log.info("Process template for entry \"" + entryId + "\"");
					const content = entry.process({
						/**
						 * The entry identifier currently being used
						 */
						entryId: entryId,
						/**
						 * The current configuration
						 */
						config: config
					});
					return createTempFileSync(config, content, (entry.getPath()) ? Path.basename(entry.getPath()) : "tmp", config.tempPath);
				}
				return entry;
			});
		}

		// ---- Resolve paths -------------------------------------------------
		Object.keys(config.entries).forEach((entryId) => {
			config.entries[entryId] = config.entries[entryId].map((path) => pathResolve(config, path));
		});
		Object.keys(config.assets).forEach((key) => {
			config.assets[key] = (config.assets[key] instanceof Array)
					? config.assets[key].map((path) => pathResolve(config, path))
					: pathResolve(config, config.assets[key]);
		});

		// ---- Handle extra config -------------------------------------------
		let webpackExtraConfig = {};
		switch (config.type) {

		case "html":
			webpackExtraConfig = Webpack.merge(webpackExtraConfig, {
				entry: config.entries
			});
			break;

		case "library":
			webpackExtraConfig = Webpack.merge(webpackExtraConfig, {
				entry: config.entries,
				output: {
					filename: "[name].js",
					library: "[name]",
					libraryTarget: "umd"
				}
			});
			break;

		case "node":
			webpackExtraConfig = Webpack.merge(webpackExtraConfig, {
				target: "node",
				entry: config.entries,
				output: {
					filename: "[name].js",
					library: "[name]",
					libraryTarget: "commonjs2"
				},
				optimization: {
					runtimeChunk: false,
					splitChunks: {
						// This is important, split chunks seems to fail on node library ending up
						// not being able to load some modules: "TypeError: Cannot read property 'call' of undefined"
						chunks: "async"
					}
				}
			});
			break;

		default:
			throw new Exception("Unsupported config type \"" + config.type + "\"");
		}

		// ---- Build webpack config ------------------------------------------
		return (env, argv) => {

			// Set the output path
			config.output = (config.output) ? config.output : argv["outputPath"];
			Exception.assert(config.output, "Output path must be set.");
			config.output = pathResolve(config, config.output);

			// If hot reloading is enabled, set a flag in the config in order to prevent some expensive operations
			config["hmr"] = (argv.hasOwnProperty("hot") && argv.hot) ? true : false;
			config["hmrCounter"] = 0;

			// Select dev or prod mode
			const isDev = (argv.mode != "production");
			Log.info("Building with webpack for " + ((isDev) ? "development" : "production") + ((config.hmr) ? " with hmr enabled" : "") + ": " + config.output);

			// Setup the webpack config increment
			let webpackConfig = Webpack.merge(getWebpackConfigDefault(isDev, config), {
				output: {
					publicPath: config.publicPath,
					path: config.output
				},
				resolve: {
					alias: config.alias,
					symlinks: false
				}
			});

			// Add extra config
			webpackConfig = Webpack.merge(webpackConfig, webpackExtraConfig);

			// ---- Remove the content of the directory -----------------------
			if (config.output) {
				Log.info("Removing content of output directory \"" + config.output + "\"");
				rmdirSync(config.output);
				Webpack.mkdirSync(config.output);
			}

			// ---- Copy common files -----------------------------------------
			for (const type in config.assets) {
				Webpack.mkdirSync(Path.resolve(config.output, "assets", type));
				config.assets[type] = config.assets[type].map((file) => {
					const fileSplit = file.split(".");
					const output = "assets/" + type + "/" + Webpack.hash(file) + "." + fileSplit[fileSplit.length - 1];
					const data = Fs.readFileSync(file);
					Fs.writeFileSync(Path.resolve(config.output, output), data);
					Log.info("Copy \"" + file + "\" -> \"" + output + "\"");
					return output;
				});
			}

			return webpackConfig;
		};
	}
};

// ---- Private methods -------------------------------------------------------

/**
 * Default configuration
 */
function getWebpackConfigDefault(isDev, config)
{
	const verboseStats = {
		all: false,
		modules: false,
		maxModules: 0,
		errors: true,
		warnings: true,
		moduleTrace: true,
		errorDetails: true,
		assets: false,
		children: false,
		chunks: false,
		chunkModules: false
	};

	const customRules = Object.keys(config.loaders).map((type) => {
		return {
			test: new RegExp("\\." + type + "$"),
			exclude: /node_modules/,
			loader: config.loaders[type]
		}
	});

	return {
		mode: (isDev) ? "development" : "production",
		// Reduce verbosity
		stats: verboseStats,
		target: "web",
		output: {
			filename: "[name].js",
			chunkFilename: "[chunkhash].js"
		},
		optimization: {
			minimize: (isDev) ? false : true,
			occurrenceOrder: true,
			runtimeChunk: "single",
			splitChunks: {
				chunks: "all"
			}
		},
		module: {
			rules: customRules.concat([
				{
					test: /\.vue$/,
					exclude: /node_modules/,
					loader: "vue-loader",
					options: {
						hotReload: isDev
					}
				},
				{
					test: /\.js$/,
					exclude: /node_modules/,
					use: "babel-loader"
				},
				{
					test: /\.(sa|sc|c)ss$/,
					use: [
						// Only apply CSS extraction for production so that you get CSS hot reload during development.
						(isDev) ? "vue-style-loader" : {
							loader: MiniCssExtractPlugin.loader,
							options: {
								publicPath: ""
							}
						},
						"css-loader",
						"sass-loader"
					]
				},
				{
					test: /\.(png|gif|jpe|jpg|svg|woff|woff2|eot|ttf)$/,
					loader: "file-loader",
					options: {
						name: "[hash].[ext]",
						outputPath: (url, resourcePath, context) => {
							let splitPath = Path.basename(resourcePath).split(".");
							return (splitPath.length > 1)
									? ("assets/" + splitPath.pop() + "/" + url)
									: ("assets/" + url)
						}
					}
				}
			])
		},
		devServer: {
			// Important, so that we can use HMR while serving file statically
			writeToDisk: true,
			compress: true,
			stats: verboseStats,
			host: "localhost",
			port: 3000,
			hotOnly: config["hmr"]
		},
		plugins: [
			new BundleAnalyzerPlugin({
				analyzerMode: "static",
				reportFilename: "./reports/webpack.html",
				openAnalyzer: false,
				logLevel: "warn"
			}),
			new VueLoaderPlugin(),
			new MiniCssExtractPlugin({
				filename: "[name].css",
				chunkFilename: "[id].css"
			}),
			new WebpackAssetsManifest({
				entrypoints: true,
				writeToDisk: false,
				output: "./reports/webpack.manifest.json",
				async done(obj) {
					// If hmr is enabled, do not process the manifest
					if (config["hmrCounter"] > 0) {
						return;
					}

					// Convert array into Object
					const entrypoints = JSON.parse(JSON.stringify(obj.get("entrypoints")));
					const manifest = await manifestCreate(config, entrypoints);

					// Generate the templates
					for (const i in config.templates) {
						let configTemplate = Object.assign({
							/**
							 * Specific entry to be used, if none, no entry object will be created.
							 */
							entryId: false,
							/**
							 * Output path where the template should be written to
							 */
							output: Path.resolve(config.output, i + ".html"),
							/**
							 * Path of the template to be used
							 */
							template: null,
							/**
							 * Content of the template to be used if not template is specified
							 */
							templateContent: "<!DOCTYPE html>"
									+ "<html>"
										+ "<head>"
											+ "<meta charset=\"utf-8\" />"
											+ "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />"
											+ "<base href=\"<% html.base %>\" />"
											+ "<% html.css %>"
										+ "</head>"
										+ "<body>"
											+ "<div id=\"app\"></div>"
											+ "<% html.js %>"
										+ "</body>"
									+ "</html>"
						}, config.templates[i]);

						// Update the path
						configTemplate.output = pathResolve(config, configTemplate.output);

						try {

							let data = configTemplate.templateContent;
							if (configTemplate.template) {
								configTemplate.template = pathResolve(config, configTemplate.template);
								data = await readFileAsync(configTemplate.template);
							}
							Exception.assert(data, "No content specified for the template");

							const template = new Template(data);

							// Pre-process HTML specific entries
							let html = {};
							if (configTemplate.entryId) {
								// Create the dependency list
								const cssList = (manifest.common.css || []).concat(manifest.entries[configTemplate.entryId].css || []);
								const jsList = (manifest.common.js || []).concat(manifest.entries[configTemplate.entryId].js || []);
								html = {
									base: manifest.path.split("/").filter((entry) => (entry && entry != "/")).map((entry) => "..").join("/") || "/",
									css: cssList.map((path) => ("<link href=\"" + manifest.path + path + "\" rel=\"stylesheet\"/>")).join(""),
									js: jsList.map((path) => ("<script src=\"" + manifest.path + path + "\"></script>")).join("")
								}
							}

							// Generate the template
							const output = template.process({
								manifest: manifest,
								manifestJSON: JSON.stringify(manifest),
								/**
								 * The current configuration object
								 */
								config: config,
								/**
								 * HTML specific parameters if needed
								 */
								html: html
							});

							await writeFileAsync(configTemplate.output, output);
							Log.info("Generated \"" + configTemplate.output + "\"" + ((configTemplate.entryId === false) ? "" : (" for entry \"" + configTemplate.entryId + "\"")));
						}
						catch (e) {
							Log.error(e);
							break;
						}
					}

					// Re-create the manifest here after the new files
					let updatedManifest = await manifestCreate(config, entrypoints);

					// Write manifest stats
					Log.info("Manifest for \"" + config.output + "\":\n" + manifestToString(config, updatedManifest));

					// Update the manifest if needed before saving it
					updatedManifest = await config.hooks.manifest(updatedManifest, config);
					await writeFileAsync(Path.resolve(config.output, "manifest.json"), JSON.stringify(updatedManifest, null, "\t"));
					Log.info("Generated \"manifest.json\"");

					// Hook at the end of the process
					await config.hooks.end(updatedManifest, config);

					// Update the hot module reloading as from now on, it is considered as enabled
					if (typeof config["hmrCounter"] === "number") {
						config["hmrCounter"]++;
					}
				}
			})
		]
	};
}

function cleanUp(config)
{
	// Delete the temporary files
	config.temp.forEach((path) => {
		Fs.unlink(path, (e) => {
			if (e) {
				// Cannot raise an exception here, otherwise we end up in an infinite loop
				Exception.fromError(e).print("Could not delete file \"" + path + "\"");
			}
		});
	});
}

async function manifestCreate(config, entrypoints)
{
	let manifest = {
		path: config.publicPath,
		common: {
			js: [],
			css: []
		},
		entries: {}
	};

	// Inject the common dependencies
	manifest = Webpack.merge(manifest, { common: config.assets });

	// Detect if an entry point is a library or not
	const isLibrary = (ep) => {
		return ((["library", "node"].indexOf(config.type) !== -1) && Object.keys(entrypoints).indexOf(ep.replace(".js", "")) !== -1);
	};

	// Inject common entries
	const getCommonEntries = () => {
		let entryList = Object.keys(entrypoints);
		// Construct the first entries object from the first element
		const entry = entryList.pop();
		let entries = Object.assign({}, entrypoints[entry]);
		// Look for common items
		entryList.forEach((entry) => {
			for (const type in entrypoints[entry]) {
				if (entries.hasOwnProperty(type)) {
					const curEntries = entrypoints[entry][type];
					entries[type] = curEntries.filter((ep) => (entries[type].indexOf(ep) !== -1));
				}
			}
		});
		// Remove any library if part of it
		entries["js"] = (entries["js"] || []).filter((ep) => !isLibrary(ep));
		return entries;
	};
	manifest = Webpack.merge(manifest, { common: getCommonEntries() });

	// Create the entries field
	for (const entryId in entrypoints) {
		manifest.entries[entryId] = {};
		for (const type in entrypoints[entryId]) {
			manifest.entries[entryId][type] = entrypoints[entryId][type].filter((ep) => (manifest.common[type].indexOf(ep) === -1 && !isLibrary(ep)));
		}
		manifest.entries[entryId]["lib"] = (entrypoints[entryId]["js"] || []).filter((ep) => isLibrary(ep));
	}

	let assignedfileList = []; 
	// Update the path by adding the publicPath and list all files
	for (const type in manifest.common) {
		assignedfileList = assignedfileList.concat(manifest.common[type]);
		manifest.common[type] = manifest.common[type].map((ep) => (ep));
	}
	for (const entry in manifest.entries) {
		for (const type in manifest.entries[entry]) {
			assignedfileList = assignedfileList.concat(manifest.entries[entry][type]);
			manifest.entries[entry][type] = manifest.entries[entry][type].map((ep) => (ep));
		}
	}

	// Include the other files
	manifest["others"] = [];
	const fileList = await readdirAsync(config.output);
	fileList.forEach((file) => {
		if (Fs.lstatSync(Path.resolve(config.output, file)).isFile()) {
			if (assignedfileList.indexOf(file) === -1) {
				manifest["others"].push(file);
			}
		}
	});

	return manifest;
}

function manifestToString(config, manifest)
{
	let stats = {};
	let maxSizes = ["path", "size", "type"].map((key) => [key, 0]);

	const bytesToString = (mem) => {
		const unitList = ["bytes", "KiB", "MiB", "GiB", "TiB"];
		let i = 0;
		for (; mem > 999 && i < unitList.length; i++) {
			mem /= 1024;
		}
		return Math.round(mem * 10) / 10 + " " + unitList[i];
	};

	const generateStats = (obj) => {
		let output = [];
		for (const type in (obj || {})) {
			output = output.concat(obj[type].map((path) => {
				const size = Fs.statSync(Path.resolve(config.output, path)).size;
				const item = {
					path: path,
					rawSize: size,
					size: bytesToString(size),
					type: type
				};
				maxSizes = maxSizes.map((value) => [value[0], Math.max(value[1], (item[value[0]] || "").toString().length)]);
				return item;
			}));
		}
		return output.sort((a, b) => a.path.localeCompare(b.path));
	};

	stats["common"] = generateStats(manifest.common);
	for (const entry in manifest.entries) {
		stats[entry] = generateStats(manifest.entries[entry]);
	}
	stats["others (async)"] = generateStats({"": manifest.others});

	// Print the results
	let outputList = ["    " + maxSizes.map((value) => value[0].padStart(value[1], " ")).join("  ")];
	for (const entry in stats) {
		outputList.push("* " + entry + " (" + bytesToString(stats[entry].reduce((sum, value) => (sum + value.rawSize), 0)) + ")");
		stats[entry].forEach((stat) => {
			outputList.push("    " + maxSizes.map((value) => (stat[value[0]] || "").toString().padStart(value[1], " ")).join("  "));
		});
	}
	return outputList.join("\n");
}

/**
 * Remove a directory recursively
 */
function rmdirSync(path)
{
	if (Fs.existsSync(path)) {
		Fs.readdirSync(path).forEach((file, index) => {
			const curPath = Path.resolve(path, file);
			if (Fs.lstatSync(curPath).isDirectory()) {
				rmdirSync(curPath);
			}
			else {
				Fs.unlinkSync(curPath);
			}
		});
		Fs.rmdirSync(path);
	}
}

/**
 * Create a temporary file and returns its name
 */
function createTempFileSync(config, data, ending, tempDirectoryPath)
{
	// Create the unique static variable
	if (typeof createTempFileSync.unique === "undefined") {
		createTempFileSync.unique = Math.floor(new Date().getTime());
		createTempFileSync.prefix = Math.floor(Math.random() * 10000);
	}

	// Create the temp directory if it does not exists
	Webpack.mkdirSync(tempDirectoryPath);

	// Generate the fileName and the path
	const fileName = ".tmp." + createTempFileSync.prefix + (createTempFileSync.unique++) + "." + ending;
	const path = Path.join(tempDirectoryPath, fileName);

	Fs.writeFileSync(path, data);

	// Add the file path the config, so that it will be deleted at the end
	config.temp.push(path);

	return path;
}

/**
 * Resolve a path, by looking at the aliases
 */
function pathResolve(config, path)
{
	let isMatch;
	do {
		isMatch = false;
		for (const alias in config.alias) {
			if (path.indexOf(alias) === 0) {
				path = path.replace(alias, config.alias[alias]);
				isMatch = true;
				break;
			}
		}
	} while (isMatch);

	// Use the correct directory separator
	return path.replace(/[\\\/]/g, Path.sep);
}

/**
 * Helper functions for promise based
 */
function readFileAsync(path)
{
	return new Promise((resolve, reject) => {
		Fs.readFile(path, (e, data) => {
			return (e) ? reject(e) : resolve(data.toString());
		});
	});
}
function writeFileAsync(path, data)
{
	return new Promise((resolve, reject) => {
		Fs.writeFile(path, data, (e) => {
			return (e) ? reject(e) : resolve();
		});
	});
}
function readdirAsync(path)
{
	return new Promise((resolve, reject) => {
		Fs.readdir(path, (e, dataList) => {
			return (e) ? reject(e) : resolve(dataList);
		});
	});
}

// ---- Exports ---------------------------------------------------------------

module.exports = Webpack;