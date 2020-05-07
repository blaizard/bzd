<template>
	<div>
		<button @click="updateValues">New values</button>
        <input type="checkbox" id="animate" v-model="animate">
        <label for="animate">animate</label>

		<h1>Simple</h1>
		<Plot class="showcase-plot" :value="values"></Plot>

		<h1>Show/Hide</h1>
        <div class="showcase-plot">
            <input type="checkbox" id="showAxisX" v-model="showAxisX">
            <label for="showAxisX">showAxisX</label>
            <input type="checkbox" id="showAxisY" v-model="showAxisY">
            <label for="showAxisY">showAxisY</label>
            <input type="checkbox" id="showCursor" v-model="showCursor">
            <label for="showCursor">showCursor</label>
            <input type="checkbox" id="showLegend" v-model="showLegend">
            <label for="showLegend">showLegend</label>
            <Plot class="showcase-plot" :config="plotConfigNoAxis" :value="values"></Plot>
        </div>
	</div>
</template>

<script>
    import Plot from "[bzd]/vue/components/graph/plot.vue"; 

	export default {
        components: {
            Plot
        },
		data: function () {
			return {
                values: [],
                counter: 0,
                showAxisX: false,
                showAxisY: false,
                showCursor: false,
                showLegend: false,
                animate: false
			}
		},
		mounted() {
			this.updateValues();
            setInterval(() => {
                if (this.animate) {
                    this.updateValues();
                }
            }, 50);
		},
        computed: {
            plotConfigNoAxis() {
                return {
                    showAxisX: this.showAxisX,
                    showAxisY: this.showAxisY,
                    showCursor: this.showCursor,
                    showLegend: this.showLegend,
                    paddingLeft: 5,
                    paddingRight: 5,
                    paddingTop: 5,
                    paddingBottom: 5
                }
            },
        },
		methods: {
            generateSin() {
                return [...Array(100).keys()].map((index) => [index + this.counter, Math.sin((index + this.counter) / 10)]);
            },
            generateRandom() {
                return [...Array(100).keys()].map((index) => [index + this.counter, Math.random() * 2 - 1]);
            },
			updateValues() {
                ++this.counter;
				this.values = [
                    {
                        caption: "Serie 1",
                        values: this.generateSin()
                    },
                    {
                        caption: "Serie 2",
                        values: this.generateRandom()
                    }
                ];
			}
		}
	}
</script>

<style lang="scss" scoped>
	.showcase-plot {
		width: 100%;
		height: 200px;
        background-color: #fff;
	}
</style>