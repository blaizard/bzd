<script>
	export default {
		data: function () {
			return {
				loading: false,
			};
		},
		emits: ["loading", "error"],
		watch: {
			loading: {
				handler(value) {
					this.$emit("loading", value);
				},
				immediate: true,
			},
		},
		computed: {
			hasListenerLoading() {
				return Boolean(this.$attrs && this.$attrs.onLoading);
			},
			hasListenerError() {
				return Boolean(this.$attrs && this.$attrs.onError);
			},
			hasGlobalNotification() {
				return Boolean(this.$notification);
			},
		},
		methods: {
			/// Callback to handle errors. If there is no "error" listener set, it defaults to calling
			/// the notification otherwise it emits an "error" event.
			///
			/// \param e The error message or exception.
			handleError(e) {
				if (this.hasListenerError) {
					this.$emit("error", e);
				} else if (this.hasGlobalNotification) {
					this.$notification.error({ [this._uid]: e });
				} else {
					console.error(e);
				}
			},
			/// Submit an action and handle the error properly.
			/// Set the loading flag during the duration of the action.
			///
			/// \param action The action to be performed.
			/// \param throwOnError If an error, throw an exception.
			async handleSubmit(action, throwOnError = false) {
				this.loading = true;
				try {
					return await action();
				} catch (e) {
					this.handleError(e);
					if (throwOnError) {
						throw e;
					}
				} finally {
					this.loading = false;
				}
			},
			/// Handler to chain the state of loading child components.
			handleLoading(isLoading) {},
		},
	};
</script>
