<template>
	<div class="bzd-layout">
		<div class="bzd-layout-header" @click.stop="hideMenu">

			<div class="bzd-layout-header-menu-trigger" @click.stop="toggleMenu">
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
                <div class="bzd-layout-menu-background" @click.stop="hideMenu" v-show="isMenuVisible">
                </div>
            </transition>
            <transition name="bzd-translate">
                <div class="bzd-layout-menu-content" @click.stop="" v-show="isMenuVisible">
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
                isMenuVisible: false
			}
		},
		mounted() {
			window.addEventListener("resize", this.handleResize, false);
			this.handleResize();
		},
		beforeDestroy() {
			window.removeEventListener("resize", this.handleResize, false);
		},
		computed: {
		},
		methods: {
			handleResize() {
			},
            toggleMenu() {
                this.isMenuVisible = !this.isMenuVisible;
            },
            hideMenu() {
                this.isMenuVisible = false;
            }
		}
	}
</script>

<style lang="scss">
    // ---- Configuration

    // Global

    // Header
    $headerColor: #fff;
    $headerBgColor: #75b9e7;
    $headerFontSize: 20px;
    $headerHeight: 44px;
    $headerPadding: ($headerHeight - $headerFontSize) / 2;
    $headerZIndex: 10;

    // Menu
    $menuColor: #000;
    $menuBgColor: #fff;
    $menuWidth: 300px;
    $menuPadding: 20px;
    $menuZIndex: 9;

    // Content
    $contentPadding: 20px;

    // ----

    $bzdIconNames: menu;
	@import "~[bzd]/icons.scss";

    $bzdColor: #000;
    $bzdOpacity: 0.5;
    @import "~[bzd]/assets/style/clickable.scss";

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

            box-shadow: 0 3px 4px 0 rgba(0, 0, 0, 0.2), 0 3px 3px -2px rgba(0, 0, 0, 0.14), 0 1px 8px 0 rgba(0, 0, 0, 0.12);

            background-color: $headerBgColor;
            color: $headerColor;

            font-size: $headerFontSize;
            line-height: $headerHeight;

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
                .bzd-menu-entry-wrapper {
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
            }
        }

        .bzd-layout-content {
            padding: $contentPadding;
        }
    }
</style>
