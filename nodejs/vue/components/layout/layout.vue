<template>
	<div :class="classLayout">
		<div class="bzd-layout-header">

			<div v-if="this.state != 'dock'"
                    class="bzd-layout-header-menu-trigger" @click.stop="toggleMenu">
                <i class="bzd-icon-menu"></i>
            </div>

			<div class="bzd-layout-header-content">
				<slot name="header"></slot>
			</div>

            <div class="bzd-layout-header-padding">
            </div>

            <div class="bzd-layout-header-actions">
                <slot name="actions"></slot>
            </div>
		</div>

		<div class="bzd-layout-menu">
            <transition name="bzd-fade">
                <div class="bzd-layout-menu-background" @click.stop="hideMenu" v-show="state == 'float'">
                </div>
            </transition>
            <transition name="bzd-translate">
                <div class="bzd-layout-menu-content" @click.stop="" v-show="state != 'hide'">
                    <div class="bzd-layout-menu-content-dock" @click="toggleDock" v-tooltip="{ text: 'Dock menu' }">
                        <i v-if="state == 'dock'" class="bzd-icon-dock-on"></i>
                        <i v-else class="bzd-icon-dock-off"></i>
                    </div>
                    <slot name="menu"></slot>
                </div>
            </transition>
		</div>

		<div class="bzd-layout-content">
			<slot name="content"></slot>
		</div>

		<div class="bzd-layout-footer">
			<slot name="footer"></slot>
		</div>
	</div>
</template>

<script>
"use strict";

import LocalStorage from "bzd/core/localstorage.mjs";
import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";

export default {
	directives: {
		tooltip: DirectiveTooltip
	},
	data: function() {
		return {
			state: LocalStorage.get("bzd-layout-state", "hide")
		};
	},
	mounted() {
		window.addEventListener("resize", this.handleResize, false);
		this.handleResize();
	},
	beforeDestroy() {
		window.removeEventListener("resize", this.handleResize, false);
	},
	computed: {
		classLayout() {
			return {
				"bzd-layout": true,
				"bzd-dock": (this.state == "dock")
			};
		}
	},
	methods: {
		handleResize() {
		},
		toggleDock() {
			this.state = (this.state == "float") ? "dock" : "hide";
			LocalStorage.set("bzd-layout-state", (this.state == "dock") ? "dock" : "hide");
		},
		toggleMenu() {
			this.state = (this.state == "hide") ? "float" : "hide";
		},
		hideMenu() {
			this.state = "hide";
		}
	}
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

    // Content
    $contentPadding: 20px;

    // ----

    @use "bzd-style/css/clickable.scss";
	@use "bzd/icons.scss" with (
        $bzdIconNames: menu dock-on dock-off
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

        .bzd-layout-content {
            padding: $contentPadding;
        }

        &.bzd-dock {
            .bzd-layout-content {
                margin-left: $menuWidth;
            }
        }
    }
</style>
