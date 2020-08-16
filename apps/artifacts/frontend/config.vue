<template>
	<div>
		<h1>Configuration for {{ volume }}</h1>
		<Form :description="formDescription" v-model="config"></Form>
		<Form :description="formDescriptionTyped" v-model="config"></Form>

		{{ config }}
	</div>
</template>

<script>
	import Plugins from "../plugins/frontend.mjs";
	import Form from "bzd/vue/components/form/form.vue";

	export default {
		props: {
			volume: { mandatory: true, type: String }
		},
		components: {
			Form
		},
		mounted() {
			this.fetchConfig();
		},
		data: function() {
			return {
				config: {}
			};
		},
		computed: {
			formDescription() {
				return [
					{
						type: "Dropdown",
						name: "type",
						list: Object.keys(Plugins).reduce((obj, key) => {
							obj[key] = Plugins[key].name;
							return obj;
						}, {})
					}
				];
			}
		},
		asyncComputed: {
			async formDescriptionTyped() {
				if (this.config.type in Plugins) {
					return await Plugins[this.config.type].form;
				}
			}
		},
		methods: {
			async fetchConfig() {
				this.config = await this.$api.request("get", "/config", {
					volume: this.volume
				});
			}
		}
	};
</script>
