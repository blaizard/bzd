import StorageWebdav from "#bzd/nodejs/db/storage/webdav.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import extensionRelease from "#bzd/apps/artifacts/plugins/extension/release/release.mjs";

export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options, provider, endpoints, [extensionRelease]);
		provider.addStartProcess(async () => {
			const storage = await StorageWebdav.make(options["webdav.url"], {
				username: options["webdav.username"],
				password: options["webdav.password"],
			});
			this.setStorage(storage);
		});
	}
}
