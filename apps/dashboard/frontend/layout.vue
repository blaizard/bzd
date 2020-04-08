<template>
	<div class="layout">
		<div class="layout-header" @click.stop="hideMenu">

			<div class="layout-header-menu-trigger" @click.stop="toggleMenu">
                <i class="bzd-icon-menu"></i>
            </div>

			<div class="layout-header-content">
				<slot name="header"></slot>
			</div>

            <div class="layout-header-actions">
                <slot name="actions"></slot>
            </div>
		</div>

		<div class="layout-menu">
            <transition name="fade">
                <div class="layout-menu-background" @click.stop="hideMenu" v-show="isMenuVisible">
                </div>
            </transition>
            <transition name="translate">
                <div class="layout-menu-content" @click.stop="" v-show="isMenuVisible">
                    <slot name="menu"></slot>
                </div>
            </transition>
		</div>

		<div class="layout-content">
			<slot name="content"></slot>
		</div>

		<div class="layout-footer">
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
    html,
    body {
        padding: 0;
		margin: 0;
    }
</style>

<style lang="scss" scoped>

    // ---- Configuration

    // Global

    // Header
    $headerColor: #fff;
    $headerBgColor: #4682bf;
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

    .layout {
        padding: 0;
		margin: 0;

        .layout-header {
            position: sticky;
			top: 0;
			left: 0;
			width: 100%;
			z-index: $headerZIndex;

            display: flex;
			flex-flow: row nowrap;
			align-items: center;
			justify-content: self-start;

            box-shadow: 0 3px 4px 0 rgba(0, 0, 0, 0.2), 0 3px 3px -2px rgba(0, 0, 0, 0.14), 0 1px 8px 0 rgba(0, 0, 0, 0.12);

            background-color: $headerBgColor;
            color: $headerColor;

            font-size: $headerFontSize;
            line-height: $headerHeight;

            padding-left: $headerPadding;
            padding-right: $headerPadding;

            > * {
                padding-left: $headerPadding / 2;
                padding-right: $headerPadding / 2;
            }

            .layout-header-menu-trigger {
                @extend %bzd-clickable;

                min-width: $headerHeight;
                text-align: center;
            }

            .layout-header-actions {
                ul {
                    padding: 0;
                    margin: 0;
                    li {
                        font-size: 0;
                        line-height: 0;
                        padding: $headerPadding;
                        i {
                            font-size: $headerFontSize;
                        }
                        list-style: none;
                        @extend %bzd-clickable;
                    }
                }
            }
        }

        .layout-menu {
            // --- Transitions

            // Fade
            .fade-enter-active,
            .fade-leave-active {
                transition: opacity 0.5s;
            }
            .fade-enter,
            .fade-leave-to {
                opacity: 0;
            }

            // Translate
            .translate-enter-active,
            .translate-leave-active {
                transition: transform 0.3s;
            }
            .translate-enter,
            .translate-leave-to {
                transform: translateX(-300px);
            }

            // ----

            position: fixed;
            z-index: $menuZIndex;

            .layout-menu-background {
                position: fixed;
                top: 0;
                left: 0;
			    width: 100vw;
                height: 100vh;

                background-color: rgba(0, 0, 0, 0.4);
            }

            .layout-menu-content {
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

        .layout-content {
            padding: $contentPadding;
        }
    }
</style>
