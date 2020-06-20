<script>
export default {
	mounted() {
		window.addEventListener("resize", this.handleResize, false);
		this.handleResize();
	},
	beforeDestroy() {
		window.removeEventListener("resize", this.handleResize, false);
	},
	data: function() {
		return {
			mobileMaxWidth: 800,
			resizeChangeCounter: 1
		};
	},
	computed: {
		isMobile() {
			const vw = Math.max(document.documentElement.clientWidth || 0, window.innerWidth || 0);
			const userAgent = navigator.userAgent || navigator.vendor || window.opera;
			return this.resizeChangeCounter > 0 && (vw <= this.mobileMaxWidth || /mobi/i.test(userAgent));
		}
	},
	methods: {
		handleResize() {
			++this.resizeChangeCounter;
		}
	}
};
</script>
