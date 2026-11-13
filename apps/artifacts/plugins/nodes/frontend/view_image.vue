<template>
	<div class="view-image" v-loading="!image && loading">
		<div
			v-if="image"
			class="view-image-frame"
			@mouseenter="$emit('hover', image.timestamp)"
			@mouseleave="$emit('hover', null)"
		>
			<div
				v-if="image.thumbnail"
				class="view-image-placeholder"
				:style="{ backgroundImage: `url(${image.thumbnail})` }"
			></div>
			<img :src="imageUrl" :class="{ loaded: loaded }" @load="loaded = true" @error="error = 'Unable to load image.'" />
			<div v-if="error" class="view-image-error">{{ error }}</div>
			<div class="view-image-info">
				<span>{{ dateToDefaultString(image.timestamp / 1000) }}</span>
				<span v-if="image.value.width && image.value.height">{{ image.value.width }}x{{ image.value.height }}</span>
			</div>
		</div>
		<div v-else-if="!loading" class="view-image-empty">No image</div>
	</div>
</template>

<script>
	import { thumbHashToDataURL } from "thumbhash";
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.js";
	import Utils from "#bzd/apps/artifacts/common/utils.js";
	import { dateToDefaultString } from "#bzd/nodejs/utils/to_string.js";

	/// The expected period between live fetches, in milliseconds.
	const livePeriodMs = 3000;

	export default {
		mixins: [Base, Component],
		directives: {
			loading: DirectiveLoading,
		},
		props: {
			options: { mandatory: true, type: Object },
			endpoint: { mandatory: true, type: String },
			timeRange: { mandatory: true, type: Array },
			configuration: { type: Object, default: () => ({}) },
			hoverTimestamp: { type: Number, default: null },
		},
		emits: ["fetch", "timestamp", "hover"],
		data: function () {
			return {
				image: null,
				loaded: false,
				error: null,
				fetching: false,
				liveTimer: null,
			};
		},
		computed: {
			/// The file endpoint is the node endpoint with the UID replaced by @file.
			fileEndpoint() {
				return this.endpoint.replace(/\/[^/]+$/, "/@file");
			},
			imageUrl() {
				if (!this.image) {
					return null;
				}
				return this.fileEndpoint + "/" + this.image.value["$file"];
			},
		},
		async mounted() {
			await this.handleSubmit(() => this.fetchImage());
			this.liveTimer = setInterval(() => this.fetchImage(), livePeriodMs);
		},
		beforeUnmount() {
			clearInterval(this.liveTimer);
		},
		methods: {
			dateToDefaultString,
			/// Convert the base64 thumbhash into a thumbnail data URL.
			makeThumbnail(thumbhash) {
				if (typeof thumbhash !== "string" || thumbhash.length === 0) {
					return null;
				}
				try {
					const bytes = Uint8Array.from(atob(thumbhash), (character) => character.charCodeAt(0));
					return thumbHashToDataURL(bytes);
				} catch (e) {
					return null;
				}
			},
			/// Fetch the newest image from all the dashboard inputs.
			async fetchImage() {
				if (this.fetching || !this.options.inputs) {
					return;
				}
				const inputs = Object.keys(this.options.inputs);
				if (inputs.length === 0) {
					this.image = null;
					return;
				}
				this.fetching = true;
				try {
					this.$emit("fetch", livePeriodMs);
					const result = await this.requestBackend(this.endpoint, {
						method: "get",
						query: {
							include: inputs.join(","),
							metadata: 1,
							count: 1,
						},
						expect: "json",
					});
					let newest = null;
					for (const [key, values] of result.data ?? []) {
						const [timestamp, value] = values[0] ?? [];
						if (timestamp === undefined || !value || typeof value["$file"] !== "string") {
							continue;
						}
						if (newest === null || timestamp > newest.timestamp) {
							newest = {
								path: Utils.keyToPath(key),
								timestamp: timestamp,
								value: value,
								thumbnail: this.makeThumbnail(value.thumbhash),
							};
						}
					}
					this.updateImage(newest);
				} finally {
					this.fetching = false;
				}
			},
			updateImage(image) {
				if (image === null) {
					this.image = null;
					return;
				}
				this.$emit("timestamp", image.timestamp);
				if (this.image && this.image.value["$file"] === image.value["$file"]) {
					return;
				}
				this.image = image;
				this.loaded = false;
				this.error = null;
			},
		},
	};
</script>

<style lang="scss" scoped>
	.view-image {
		height: 100%;
		background-color: #111;
		overflow: hidden;

		.view-image-frame {
			position: relative;
			width: 100%;
			height: 100%;
		}

		.view-image-placeholder {
			position: absolute;
			inset: -10px;
			background-position: center;
			background-size: cover;
			filter: blur(10px);
			transform: scale(1.05);
		}

		img {
			position: absolute;
			inset: 0;
			width: 100%;
			height: 100%;
			object-fit: contain;
			opacity: 0;
			transition: opacity 0.2s;

			&.loaded {
				opacity: 1;
			}
		}

		.view-image-info {
			position: absolute;
			left: 0;
			right: 0;
			bottom: 0;
			display: flex;
			gap: 12px;
			padding: 6px 10px;
			color: #fff;
			font-size: 12px;
			background: linear-gradient(transparent, rgba(0, 0, 0, 0.7));
		}

		.view-image-error,
		.view-image-empty {
			position: absolute;
			inset: 0;
			display: flex;
			align-items: center;
			justify-content: center;
			color: #888;
		}
	}
</style>
