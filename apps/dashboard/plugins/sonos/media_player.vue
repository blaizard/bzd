<template>
	<div class="media-player" ref="container" :style="containerStyle">
		<div class="title" ref="title">{{ title }}</div>
		<div class="state clickable" @click="onStateClick">
			<i v-if="metadata.state == 'play'" class="bzd-icon-pause"></i>
			<i v-else class="bzd-icon-play"></i>
		</div>
	</div>
</template>

<script>
	export default {
		props: {
			metadata: { type: Object, mandatory: true },
		},
		data: function () {
			return {
				titleScroll: 0,
			};
		},
		computed: {
			title() {
				if (this.metadata.artist) {
					return this.metadata.artist + " - " + this.metadata.title;
				}
				return this.metadata.title;
			},
			containerStyle() {
				return {
					"--title-scroll-start": (this.titleScroll) ? "10px" : 0,
					"--title-scroll-end": (this.titleScroll) ? ((this.titleScroll - 10) + "px") : 0,
					"--title-scroll-speed": -this.titleScroll / 20 + "s",
				};
			},
		},
		watch: {
			title: {
				immediate: true,
				async handler() {
					await this.$nextTick();
					const container = this.$refs.container.getBoundingClientRect();
					const rect = this.$refs.title.getBoundingClientRect();
					this.titleScroll = -Math.max(0, rect.width - container.width);
				},
			},
		},
		methods: {
			async onStateClick() {
				if (this.metadata.state == "pause") {this.$emit("event", "play");}
				else {this.$emit("event", "pause");}
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
        $bzdIconNames: play pause stop
    );
	@use "bzd-style/css/clickable.scss";

	.media-player {
		--title-scroll-start: 0;
		--title-scroll-end: 0;
		--title-scroll-speed: 5s;

		.clickable {
			@extend %bzd-clickable;
		}

		.title {
			white-space: nowrap;
			position: absolute;
			animation: marquee var(--title-scroll-speed) linear infinite;
		}

		@keyframes marquee {
			0% {
				left: var(--title-scroll-start);
			}
			50% {
				left: var(--title-scroll-end);
			}
			100% {
				left: var(--title-scroll-start);
			}
		}

		.state {
			text-align: center;
			position: absolute;
			top: calc(50% - 20px);
			left: calc(50% - 30px);
			width: 60px;
			height: 60px;
			line-height: 60px;
			font-size: 56px;
			border-radius: 5px;
		}
	}
</style>
