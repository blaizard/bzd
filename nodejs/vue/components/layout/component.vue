<script>
	export default {
		data: function () {
			return {
				loading: false,
			};
		},
		emits: ["loading"],
		methods: {
			handleError(e) {
				if (e !== false) {
					this.$notification.error({ [this._uid]: e });
				}
			},
			// Submit an action and handle the error properly.
			// Set the loading flag during the duration of the action.
			async handleSubmit(action, throwOnError = false) {
				this.loading = true;
				this.$emit("loading", true);
				try {
					return await action();
				} catch (e) {
					this.handleError(e);
					if (throwOnError) {
						throw e;
					}
				} finally {
					this.loading = false;
					this.$emit("loading", false);
				}
			},
		},
	};
</script>
