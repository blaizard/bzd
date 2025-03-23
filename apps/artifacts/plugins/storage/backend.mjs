import makeFromConfig from "#bzd/nodejs/db/storage/make_from_config.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import extensionRelease from "#bzd/apps/artifacts/plugins/extension/release/release.mjs";
import extensionUpload from "#bzd/apps/artifacts/plugins/extension/upload/upload.mjs";

/// The storage plugin support the following features:
///
/// - upload:
///   Allow certain directory to be uploadable, this also comes with several options.
/// - release:
///   Allow certain directory to be used as release depot. This creates an endpoint to get a release if available.
export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options, provider, endpoints, [extensionRelease, extensionUpload]);
		provider.addStartProcess(async () => {
			const storage = await makeFromConfig(options["storage"]);
			this.setStorage(storage);
		});
	}
}
