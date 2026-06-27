import KeyValueStoreDisk from "#bzd/nodejs/db/key_value_store/disk.js";
import KeyValueStoreElasticsearch from "#bzd/nodejs/db/key_value_store/elasticsearch.js";
import KeyValueStoreMemory from "#bzd/nodejs/db/key_value_store/memory.js";
import KeyValueStoreMongodb from "#bzd/nodejs/db/key_value_store/mongodb.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Validation from "#bzd/nodejs/core/validation.js";

const Exception = ExceptionFactory("db", "kvs", "make-from-config");

function validateConfig(schema, config) {
	const validation = new Validation(schema);
	const result = validation.validate(config, { output: "result", all: true });
	Exception.assertResult(result);
}

export default async function makeFromConfig(config) {
	switch (config.type) {
		case "memory":
			validateConfig(
				{
					type: "mandatory",
					name: "mandatory",
					options: "",
				},
				config,
			);
			return await KeyValueStoreMemory.make(config.name, config.options);
		case "disk":
			validateConfig(
				{
					type: "mandatory",
					path: "mandatory",
					options: "",
				},
				config,
			);
			return await KeyValueStoreDisk.make(config.path, config.options);
		case "elasticsearch":
			validateConfig(
				{
					type: "mandatory",
					host: "mandatory",
					options: "",
				},
				config,
			);
			return await KeyValueStoreElasticsearch.make(config.host, config.options);
		case "mongodb":
			validateConfig(
				{
					type: "mandatory",
					uri: "mandatory",
					options: "",
				},
				config,
			);
			return await KeyValueStoreMongodb.make(config.uri, config.options);
		default:
			Exception.unreachable("Unsupported key value store type: '{}'.", config.type);
	}
}
