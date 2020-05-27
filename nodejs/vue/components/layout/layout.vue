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
                    <div class="bzd-layout-menu-content-pin" @click="toggleDock">
                        <i v-if="state == 'dock'" class="bzd-icon-pin-on"></i>
                        <i v-else class="bzd-icon-pin-off"></i>
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
export default {
	props: {
	},
	components: {
	},
	data: function() {
		return {
            state: "hide"
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
    $headerFontSize: 20px;
    $headerHeight: 44px;
    $headerPadding: ($headerHeight - $headerFontSize) / 2;
    $headerZIndex: 10;

    // Menu
    $menuColor: colors.$bzdGraphColorBlack;
    $menuBgColor: colors.$bzdGraphColorWhite;
    $menuWidth: 300px;
    $menuPadding: 20px;
    $menuZIndex: 9;

    // Content
    $contentPadding: 20px;

    // ----

    @use "bzd-style/css/clickable.scss";
	@use "bzd/icons.scss" with (
        $bzdIconNames: menu pin-on pin-off
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

            .bzd-layout-header-menu-trigger {
                @extend %bzd-clickable;

                min-width: $headerHeight;
                text-align: center;
            }

            .bzd-layout-header-content {
                padding-left: $headerPadding / 2;
                padding-right: $headerPadding / 2;
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

                background-color: $menuBgColor;
                color: $menuColor;
                padding: $menuPadding;
                padding-top: $headerHeight + $menuPadding;

                .bzd-layout-menu-content-pin {
                    position: absolute;
                    right: 0;
                    top: $headerHeight;
                    padding: .3em;
                    @extend %bzd-clickable;
                }

                .bzd-menu-entry-wrapper {
                    display: flex;
                    flex-flow: column;

                    .bzd-menu-entry {
                        cursor: pointer;
                        &:hover {
                            background-color: #eee;
                        }
                    }

                    .bzd-menu-nested {
                        padding-left: 10px;
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
            .bzd-layout-menu-content {
                border-right: 1px solid colors.$bzdGraphColorBlack;
            }
        }
    }
</style>
