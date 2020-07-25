<template>
	<div :class="classLayout">
		<div class="bzd-layout-header" @click.stop="hideMenu">
			<div v-if="isMenu && (isMobile || !isDock)" class="bzd-layout-header-menu-trigger" @click.stop="toggleMenu">
				<i class="bzd-icon-menu"></i>
			</div>

			<div class="bzd-layout-header-content">
				<slot name="header"></slot>
			</div>

			<div class="bzd-layout-header-padding"></div>

			<div class="bzd-layout-header-actions" v-if="!isMobile">
				<slot name="actions"></slot>
			</div>
		</div>

		<div class="bzd-layout-menu" v-show="isMenu">
			<transition name="bzd-fade">
				<div
					class="bzd-layout-menu-background"
					@click.stop="hideMenu"
					v-show="isMenuShow && !isDock && !isMobile"></div>
			</transition>
			<transition name="bzd-translate">
				<div class="bzd-layout-menu-content" @click.stop="hideMenu" v-show="isMenuShow || (isDock && !isMobile)">
					<div class="bzd-layout-menu-content-dock" @click="toggleDock" v-tooltip="{ text: 'Dock menu' }">
						<i v-if="isDock" class="bzd-icon-dock_on"></i>
						<i v-else class="bzd-icon-dock_off"></i>
					</div>
					<slot name="actions" v-if="isMobile"></slot>
					<slot name="menu"></slot>
				</div>
			</transition>
		</div>

		<div class="bzd-layout-content-wrapper">
			<template v-if="isNotification">
				<div
					v-for="entry in $notification.entries"
					:key="entry.key"
					:class="getNotificationClass(entry)"
					:style="getNotificationStyle(entry)">
					<div class="bzd-notification-content">{{ entry.message }}</div>
					<div class="bzd-notification-close" @click="notificationClose(entry)">
						<i class="bzd-icon-close"></i>
					</div>
				</div>
			</template>

			<div class="bzd-layout-content">
				<slot name="content"></slot>
			</div>
		</div>

		<div class="bzd-layout-footer">
			<slot name="footer"></slot>
		</div>
	</div>
</template>

<script>
	import Base from "./base.vue";
	import LocalStorage from "bzd/core/localstorage.mjs";
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";

	export default {
		mixins: [Base],
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				isDock: Boolean(LocalStorage.get("bzd-layout-dock", false)),
				isMenuShow: false,
			};
		},
		computed: {
			isNotification() {
				return typeof this.$notification == "object";
			},
			classLayout() {
				return {
					"bzd-layout": true,
					"bzd-mobile": this.isMobile,
					"bzd-dock": this.isDock && !this.isMobile && this.isMenu,
				};
			},
			isMenu() {
				return Boolean(this.$slots.menu) || (this.isMobile && Boolean(this.$slots.actions));
			},
		},
		methods: {
			getNotificationClass(entry) {
				return {
					"bzd-notification": true,
					"bzd-notification-time": entry.timeOnScreen > 0,
					"bzd-success": entry.type == "success",
					"bzd-error": entry.type == "error",
					"bzd-info": entry.type == "info",
				};
			},
			getNotificationStyle(entry) {
				if (entry.timeOnScreen) {
					return "--bzd-notification-time: " + entry.timeOnScreen + "s;";
				}
				return "";
			},
			notificationClose(entry) {
				this.$notification.close(entry);
			},
			toggleDock() {
				this.isDock = !this.isDock;
				this.isMenuShow = false;
				LocalStorage.set("bzd-layout-dock", this.isDock ? "1" : "");
			},
			toggleMenu() {
				this.isMenuShow = !this.isMenuShow;
			},
			hideMenu() {
				this.isMenuShow = false;
			},
			handleMenuClick() {
				this.hideMenu();
			},
		},
	};
</script>

<style lang="scss">
	// ---- Configuration
	@use "bzd-style/css/colors.scss" as colors;

	// Global

	// Header
	$headerColor: colors.$bzdGraphColorWhite;
	$headerBgColor: colors.$bzdGraphColorBlue;
	$headerFontSize: 22px;
	$headerHeight: 56px;
	$headerPadding: ($headerHeight - $headerFontSize) / 2;
	$headerZIndex: 10;

	// Menu
	$menuColor: colors.$bzdGraphColorBlack;
	$menuBgColor: colors.$bzdGraphColorWhite;
	$menuWidth: 300px;
	$menuPadding: 16px;
	$menuZIndex: 9;

	// ----

	@use "bzd-style/css/clickable.scss";
	@use "bzd/icons.scss" with (
        $bzdIconNames: menu dock_on dock_off close
    );

	html,
	body {
		padding: 0;
		margin: 0;
	}

	.bzd-layout {
		padding: 0;
		margin: 0;

		.bzd-layout-header {
			position: sticky;
			top: 0;
			left: 0;
			width: 100%;
			z-index: $headerZIndex;

			display: flex;
			flex-flow: row nowrap;
			align-items: center;
			justify-content: space-between;

			background-color: $headerBgColor;
			color: $headerColor;

			font-size: $headerFontSize;
			line-height: $headerHeight;

			a {
				color: $headerColor !important;
			}

			> *:first-child {
				margin-left: $headerPadding / 2;
			}

			.bzd-layout-header-menu-trigger {
				margin-left: 0 !important;
				@extend %bzd-clickable;

				min-width: $headerHeight;
				text-align: center;
			}

			.bzd-layout-header-content {
				padding-left: $headerPadding / 2;
				padding-right: $headerPadding / 2;
				white-space: nowrap;
			}

			.bzd-layout-header-padding {
				width: 100%;
			}

			.bzd-layout-header-actions {
				display: flex;
				flex-flow: row nowrap;

				> * {
					min-width: $headerHeight;
					text-align: center;
				}

				.bzd-menu-nested,
				.bzd-menu-entry-text {
					display: none;
				}
				.bzd-menu-entry {
					width: 100%;
					height: 100%;
					@extend %bzd-clickable;
				}
			}
		}

		.bzd-layout-menu {
			// --- Transitions

			// Fade
			.bzd-fade-enter-active,
			.bzd-fade-leave-active {
				transition: opacity 0.5s;
			}
			.bzd-fade-enter,
			.bzd-fade-leave-to {
				opacity: 0;
			}

			// Translate
			.bzd-translate-enter-active,
			.bzd-translate-leave-active {
				transition: transform 0.3s;
			}
			.bzd-translate-enter,
			.bzd-translate-leave-to {
				transform: translateX(-300px);
			}

			// ----

			position: fixed;
			z-index: $menuZIndex;

			.bzd-layout-menu-background {
				position: fixed;
				top: 0;
				left: 0;
				width: 100vw;
				height: 100vh;

				background-color: rgba(0, 0, 0, 0.4);
			}

			.bzd-layout-menu-content {
				position: fixed;
				top: 0;
				left: 0;
				width: $menuWidth;
				height: 100vh;
				padding-top: $headerHeight;

				background-color: $menuBgColor;
				color: $menuColor;
				border-right: 1px solid rgba(0, 0, 0, 0.4);

				.bzd-layout-menu-content-dock {
					position: absolute;
					right: 0;
					top: $headerHeight;
					line-height: 2em;
					height: 2em;
					width: 2em;
					text-align: center;
					@extend %bzd-clickable;
				}

				.bzd-menu-entry-wrapper {
					display: flex;
					flex-flow: column;

					.bzd-menu-entry {
						padding: $menuPadding;
						cursor: pointer;
						&:hover {
							background-color: #eee;
						}

						.bzd-menu-entry-icon {
							margin: 0 $menuPadding / 2;
						}
					}

					.bzd-menu-nested {
						.bzd-menu-entry {
							padding: 2px;
							padding-left: $menuPadding + 10px;
						}
					}
				}
			}
		}

		.bzd-layout-content-wrapper {
			.bzd-notification {
				display: flex;
				flex-flow: row nowrap;
				position: relative;

				&.bzd-notification-time {
					@keyframes bzd-notification-time-animation {
						0% {
							left: -100%;
						}
						100% {
							left: 0;
						}
					}

					&:after {
						position: absolute;
						top: 0;
						left: 0;
						height: 100%;
						width: 100%;
						opacity: 0.2;
						background-color: colors.$bzdGraphColorWhite;
						animation-name: bzd-notification-time-animation;
						animation-duration: var(--bzd-notification-time);
						animation-timing-function: linear;
						pointer-events: none;
						content: " ";
					}
				}

				> * {
					padding: 10px;
				}
				.bzd-notification-content {
					flex: 1;
				}
				.bzd-notification-close {
					@extend %bzd-clickable;
				}

				&.bzd-info {
					background-color: colors.$bzdGraphColorYellow;
					color: colors.$bzdGraphColorBlack;
				}
				&.bzd-success {
					background-color: colors.$bzdGraphColorGreen;
					color: colors.$bzdGraphColorWhite;
				}
				&.bzd-error {
					background-color: colors.$bzdGraphColorRed;
					color: colors.$bzdGraphColorWhite;
				}
			}

			.bzd-layout-content {
				padding: 0;
			}
		}

		&.bzd-dock {
			.bzd-layout-content-wrapper {
				margin-left: $menuWidth;
			}
		}

		&.bzd-mobile {
			.bzd-layout-menu {
				.bzd-translate-enter-active,
				.bzd-translate-leave-active {
					transition: none;
				}
				.bzd-translate-enter,
				.bzd-translate-leave-to {
					transform: none;
				}

				.bzd-layout-menu-content {
					width: 100%;
					border: none;
					.bzd-layout-menu-content-dock {
						display: none;
					}
				}
			}
		}
	}

	// Only show the content div in print mode
	@media print {
		.bzd-layout-header,
		.bzd-layout-menu,
		.bzd-layout-footer {
			display: none !important;
		}

		.bzd-layout-content-wrapper {
			padding: 0 !important;
			margin: 0 !important;
		}
	}
</style>
