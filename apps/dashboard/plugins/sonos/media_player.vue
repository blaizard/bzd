<template>
	<div class="media-player" ref="container" :style="containerStyle">
		<div class="title" ref="title">{{ title }}</div>
		<div class="state" @click="onStateClick">
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
				titleScroll: 0
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
					"--title-scroll": this.titleScroll + "px",
					"--title-scroll-speed": (-this.titleScroll / 20) + "s"
				}
			}
		},
		watch: {
			title: {
				immediate: true,
				async handler() {
					await this.$nextTick();
					const container = this.$refs.container.getBoundingClientRect();
					const rect = this.$refs.title.getBoundingClientRect();
					this.titleScroll = -Math.max(0, rect.width - container.width);
					console.log(this.titleScroll);
				}
			}
		},
		methods: {
			async onStateClick() {
				if (this.metadata.state == "pause")
					this.$emit("event", "play");
				else
					this.$emit("event", "pause");
			}
		}
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
        $bzdIconNames: play pause stop
    );

	.media-player {
		--title-scroll: -100%;
		--title-scroll-speed: 5s;

		.title {
			white-space: nowrap;
			position: absolute;
			animation: marquee var(--title-scroll-speed) linear infinite;
		}

		@keyframes marquee {
			0% { left: 0; }
			50% { left: var(--title-scroll); }
			100% { left: 0%; }
		}

		.state {
			text-align: center;
			position: absolute;
			top: 35%;
			font-size: 56px;
			width: 100%;
			cursor: pointer;
		}
	}
</style>
