import makeFromConfig from "#bzd/nodejs/db/storage/make_from_config.js";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.js";
import extensionRelease from "#bzd/apps/artifacts/plugins/extension/release/release.js";
import extensionUpload from "#bzd/apps/artifacts/plugins/extension/upload/upload.js";
import extensionWebdav from "#bzd/apps/artifacts/plugins/extension/webdav/webdav.js";
import extensionCachingProxy from "#bzd/apps/artifacts/plugins/extension/caching_proxy/caching_proxy.js";

/// The storage plugin support the following features:
///
/// - upload:
///   Allow certain directory to be uploadable, this also comes with several options.
/// - release:
///   Allow certain directory to be used as release depot. This creates an endpoint to get a release if available.
/// - webdav
///   Create a webdav server to access the files.
export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options, provider, endpoints, [
			extensionRelease,
			extensionUpload,
			extensionWebdav,
			extensionCachingProxy,
		]);
		provider.addStartProcess(volume + ".constructor", async () => {
			const storage = await makeFromConfig(options.storage);
			this.setStorage(storage);
		});
	}
}
