import format from "#bzd/nodejs/core/format.mjs";

export default {
	group: {
		format: (group, options) => {
			return format(options, group);
		},
	},
};
