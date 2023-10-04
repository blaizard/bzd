<script>
	export default {
		data: function () {
			return {
				loading: false,
			};
		},
		computed: {
			redirect() {
				return typeof URLSearchParams !== "undefined"
					? new URLSearchParams(window.location.search).get("redirect")
					: null;
			},
			profile() {
				if (this.$authentication.isAuthenticated) {
					return this.$cache.getReactive("profile", "default");
				}
				return {};
			},
			userLogStr() {
				return "**** User ****\n" + JSON.stringify(this.profile, undefined, 4);
			},
			navigatorLogStr() {
				const info = {
					cookieEnabled: navigator.cookieEnabled,
					isOnline: navigator.onLine,
					appName: navigator.appName,
					product: navigator.product,
					appVersion: navigator.appVersion,
					userAgent: navigator.userAgent,
					platform: navigator.platform,
					language: navigator.language,
				};

				return "**** Navigator ****\n" + JSON.stringify(info, undefined, 4);
			},
		},
		methods: {
			handleError(e) {
				if (e !== false) {
					/*this.$notification.error(e, {
						actions: [
							{
								html: "<i class=\"bzd-icon-report\"></i> Report",
								callback: (entry) => {
									// Build the message report
									let message =
										"---- Please describe here (if possible) how to reproduce this issue, the more details we have the faster we can fix it! ----\n\n";
									message += "**** Error Report ****\n";
									message += "- String: " + String(entry.raw) + "\n";
									if ("stack" in entry.raw) {
										message += "- Stack:\n";
										message += entry.raw.stack + "\n";
									}
									message += this.userLogStr + "\n";
									message += this.navigatorLogStr + "\n";

									this.$routerDispatch("/support", {
										props: {
											category: "technical",
											message: message,
										},
									});

									this.$notification.close(entry);
								},
							},
							...this.$notification.defaultActions,
						],
					});*/
					console.error(e);
				}
			},
			async handleSubmit(action) {
				this.loading = true;
				try {
					console.log(action);
					return await action();
				} catch (e) {
					this.handleError(e);
				} finally {
					this.loading = false;
				}
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/icons.scss" as * with (
		$bzdIconNames: report
	);
</style>
