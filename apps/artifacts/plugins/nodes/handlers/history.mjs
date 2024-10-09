export default {
	process(options) {
		return Math.max(0, ...options) || 10;
	},
};
