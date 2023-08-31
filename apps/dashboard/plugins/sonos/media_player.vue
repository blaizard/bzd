<template>
	<div class="media-player" ref="container" :style="containerStyle">
		<div class="title" ref="title">{{ title }}</div>
		<div class="previous" @click="onPreviousClick" @mouseover="handleOver" @mouseleave="handleLeave">
			<i class="bzd-icon-previous"></i>
		</div>
		<div class="state" @click="onStateClick" @mouseover="handleOver" @mouseleave="handleLeave">
			<i v-if="metadata.state == 'play'" class="bzd-icon-pause"></i>
			<i v-else class="bzd-icon-play"></i>
		</div>
		<div class="next" @click="onNextClick" @mouseover="handleOver" @mouseleave="handleLeave">
			<i class="bzd-icon-next"></i>
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
			name() {
				return this.metadata.name;
			},
			image() {
				return this.metadata.art;
			},
			containerStyle() {
				return {
					"--title-scroll-start": this.titleScroll ? "10px" : 0,
					"--title-scroll-end": this.titleScroll ? this.titleScroll - 10 + "px" : 0,
					"--title-scroll-speed": -(this.titleScroll - 20) / 20 + "s",
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
			name: {
				immediate: true,
				async handler() {
					this.$emit("name", this.name);
				},
			},
			image: {
				immediate: true,
				async handler() {
					this.$emit("image", this.image);
				},
			},
		},
		methods: {
			onStateClick() {
				if (this.metadata.state == "pause") {
					this.$emit("event", "play");
				}
				else {
					this.$emit("event", "pause");
				}
			},
			onNextClick() {
				this.$emit("event", "next");
			},
			onPreviousClick() {
				this.$emit("event", "previous");
			},
			handleOver() {
				this.$emit("clickable", true);
			},
			handleLeave() {
				this.$emit("clickable", false);
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/icons.scss" as * with (
		$bzdIconNames: play pause stop next previous
	);

	.media-player {
		--title-scroll-start: 0;
		--title-scroll-end: 0;
		--title-scroll-speed: 5s;

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

		.state,
		.next,
		.previous {
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

		.next {
			left: calc(50% + 40px);
		}

		.previous {
			left: calc(50% - 100px);
		}
	}
</style>
