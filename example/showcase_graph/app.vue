<template>
	<div>
		<button @click="updateValues">New values</button>
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
                showAxisX: false,
                showAxisY: false,
                showCursor: false,
                showLegend: false
			}
		},
		mounted() {
			this.updateValues();
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
			updateValues() {
				this.values = [
                    {
                        caption: "Serie 1",
                        values: [...Array(100).keys()].map((index) => [index, Math.sin(index / 10)]) //[...Array(100).keys()].map((index) => [index, Math.random()])
                    },
                    {
                        caption: "Serie 2",
                        values: [...Array(100).keys()].map((index) => [index, Math.random() * 2 - 1])
                    }
                ];
			}
		}
	}
</script>

<style lang="scss">
    body {
        background-color: #eee;
    }
</style>

<style lang="scss" scoped>
	.showcase-plot {
		width: 100%;
		height: 200px;
        background-color: #fff;
	}
</style>