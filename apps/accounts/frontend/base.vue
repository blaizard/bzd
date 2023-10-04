<script>
	import { HttpClientException } from "#bzd/nodejs/core/http/client.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";

	export default {
		mixins: [Component],
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
					// Handle expected errors.
					if (e instanceof HttpClientException) {
						if (e.code == 401 /*Unauthorized*/) {
							Component.methods.handleError.call(this, "Unauthorized");
							return;
						}
					}

					// Unexpected errors.
					this.$notification.error("message" in e ? e.message : e, {
						actions: [
							{
								html: '<i class="bzd-icon-report"></i> Report',
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
					});
					console.error(e);
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
