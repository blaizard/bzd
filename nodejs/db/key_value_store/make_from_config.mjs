import KeyValueStoreDisk from "#bzd/nodejs/db/key_value_store/disk.mjs";
import KeyValueStoreElasticsearch from "#bzd/nodejs/db/key_value_store/elasticsearch.mjs";
import KeyValueStoreMemory from "#bzd/nodejs/db/key_value_store/memory.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";

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
		default:
			Exception.unreachable("Unsupported key value store type: '{}'.", type);
	}
}
