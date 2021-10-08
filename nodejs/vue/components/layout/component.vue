<script>
	export default {
		data: function() {
			return {
				loading: false
			};
		},
		methods: {
			handleError(e) {
				if (e !== false) {
					this.$notification.error({ [this._uid]: e });
				}
			},
			async handleSubmit(action, throwOnError = false) {
				this.loading = true;
				try {
					return await action();
				}
				catch (e) {
					this.handleError(e);
					if (throwOnError) {
						throw e;
					}
				}
				finally {
					this.loading = false;
				}
			}
		}
	};
</script>
