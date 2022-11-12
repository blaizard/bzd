<template>
	<div class="camera">
		<div v-if="!supported">Camera not supported!</div>
		<video ref="video" autoplay="true"></video>
	</div>
</template>

<script>
	export default {
		data: function () {
			return {};
		},
		mounted() {
			if (this.supported) {
				this.setupCamera();
			}
		},
		computed: {
			supported() {
				return "mediaDevices" in navigator && "getUserMedia" in navigator.mediaDevices;
			},
		},
		methods: {
			async setupCamera() {
				const constraints = {
					video: {
						width: {
							min: 640,
						},
						height: {
							min: 480,
						},
					},
					audio: false,
				};
				const videoStream = await navigator.mediaDevices.getUserMedia(constraints);
				this.$refs.video.srcObject = videoStream;
			},
		},
	};
</script>

<style lang="scss" scoped>
	.camera {
		background-color: green;
		margin: 0;
		padding: 0;

		video {
			width: 100%;
			height: 100%;
			object-fit: cover;
		}
	}
</style>
