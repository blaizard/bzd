<template>
	<div>
		<button @click="record">Record</button>
		<button @click="stop">Stop</button>
		<div class="camera">
			<div v-if="!supported">Camera not supported!</div>
			<video ref="video" autoplay="true" muted="true"></video>
		</div>
		<canvas ref="canvas"></canvas>
	</div>
</template>

<script>
	async function startCapture(displayMediaOptions) {
		let captureStream = null;

		try {
			captureStream = await navigator.mediaDevices.getDisplayMedia(displayMediaOptions);
		} catch (err) {
			console.error("Error: " + err);
		}
		return captureStream;
	}

	async function streamDownloadDecryptToDisk(stream, outputStream) {
		stream.ondataavailable = async function (e) {
			if (e.data.size > 0) {
				await outputStream.write(e.data);
			}
		};

		/*
        // create transform stream for decryption
        let ts_dec = new TransformStream({
            async transform(chunk, controller) {
                controller.enqueue(chunk);
            }
        });

        // stream cleartext to file
        let rs_clear = stream.then(s => s.pipeThrough(ts_dec));
        return (await rs_clear).pipeTo(await ws_dest);
*/
	}

	async function createRecorder(stream, outputStream, mimeType) {
		// the stream data is stored in this array
		let recordedChunks = [];

		const mediaRecorder = new MediaRecorder(stream);

		mediaRecorder.ondataavailable = function (e) {
			if (e.data.size > 0) {
				recordedChunks.push(e.data);
			}
		};
		mediaRecorder.onstop = function () {
			saveFile(recordedChunks);
			recordedChunks = [];
		};
		mediaRecorder.start(200); // For every 200ms the stream data will be stored in a separate chunk.

		await streamDownloadDecryptToDisk(mediaRecorder, outputStream);

		return mediaRecorder;
	}

	function download(fileStream) {
		/* const blob = new Blob(fileStream, {
            type: 'video/webm'
        });*/
		let downloadLink = document.createElement("a");
		downloadLink.href = URL.createObjectURL(fileStream);
		downloadLink.download = "recording.webm";

		document.body.appendChild(downloadLink);
		downloadLink.click();
		URL.revokeObjectURL(blob);
		document.body.removeChild(downloadLink);
	}

	function saveFile(recordedChunks) {
		const blob = new Blob(recordedChunks, {
			type: "video/webm",
		});
		let downloadLink = document.createElement("a");
		downloadLink.href = URL.createObjectURL(blob);
		downloadLink.download = "recording.webm";

		document.body.appendChild(downloadLink);
		downloadLink.click();
		URL.revokeObjectURL(blob);
		document.body.removeChild(downloadLink);
	}

	/*

https://codepen.io/BRAiNCHiLD95/pen/BaJZOyR

1. Fetch Webcam Stream via getUserMedia()
2. Fetch Screen Share Stream via getDisplayMedia()
3. Merge Both Stream using some math & canvas operations
4. Use canvas.captureStream() to generate the composite video stream.
5. Use AudioContext to merge audio clips (especially needed if using both microphone & system audio).
6. Use MediaStream constructor to create a new stream using - the video from the new stream + audio from audioContext Destination Node as follows -

    */

	export default {
		methods: {
			async record() {
				const fileHandle = await window.showSaveFilePicker();
				this.outputStream = await fileHandle.createWritable();

				const screenStream = await navigator.mediaDevices.getDisplayMedia(this.displayMediaOptions);
				const webcamStream = await navigator.mediaDevices.getUserMedia({ audio: true /*, video: true*/ });

				// stream is of MediaStream type
				let newStream = new MediaStream();

				// getTracks method returns an array of all stream inputs
				// within a MediaStream object, in this case we have
				// two tracks, an audio and a video track
				screenStream.getTracks().forEach((track) => newStream.addTrack(track));
				webcamStream.getTracks().forEach((track) => newStream.addTrack(track));

				this.$refs.video.srcObject = newStream;
				this.mediaRecorder = await createRecorder(newStream, this.outputStream, "video/webm");
			},
			async stop() {
				this.mediaRecorder.stop();
				await this.outputStream.close();
			},
		},
		mounted() {
			/* window.onbeforeunload = function(e) {
                //if( download_is_done() ) return

                var dialogText = 'Download is not finish, leaving the page will abort the download'
                e.returnValue = dialogText
                return dialogText
            }*/
		},
		data: function () {
			return {
				mediaRecorder: null,
				outputStream: null,
			};
		},
		computed: {
			supported() {
				return "mediaDevices" in navigator && "getUserMedia" in navigator.mediaDevices;
			},
			displayMediaOptions() {
				return {
					video: { mediaSource: "screen" },
					audio: true,
				};
			},
		},
	};
</script>

<style lang="scss" scoped>
	.camera {
		width: 300px;
		height: 200px;

		video {
			width: 100%;
		}
	}
</style>
