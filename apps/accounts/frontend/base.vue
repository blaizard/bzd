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
						if (e.code == 429 /*Too Many Requests*/) {
							Component.methods.handleError.call(this, "Too Many Requests");
							return;
						}
					}

					// Read the actual message.
					const message = e && typeof e == "object" && "message" in e ? e.message : e;

					// Unexpected errors.
					this.$notification.error(message, {
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

									this.$router.dispatch("/support", {
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
	@use "@/nodejs/icons.scss" as * with (
		$bzdIconNames: report
	);
</style>
