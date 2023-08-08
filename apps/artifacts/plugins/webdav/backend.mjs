import StorageWebdav from "bzd/db/storage/webdav.mjs";

export default {
  async storage(params) {
	return await StorageWebdav.make(params["webdav.url"], {
	  username : params["webdav.username"],
	  password : params["webdav.password"],
	});
  },
};
