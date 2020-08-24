<template>
	<div>
		<h1>Configuration for {{ volume }}</h1>
		<Form :description="formDescription" v-model="config"></Form>
		<Form :description="formDescriptionTyped || []" v-model="config" @submit="handleSubmit"></Form>
	</div>
</template>

<script>
	import Plugins from "../plugins/frontend.mjs";
	import Form from "bzd/vue/components/form/form.vue";

	export default {
		props: {
			volume: { mandatory: false, type: String, default: null }
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
					const description = await Plugins[this.config.type].form;
					return description.concat([
						{
							type: "Button",
							action: "approve"
						},
						{
							type: "Button",
							action: "danger",
							content: "Delete",
							click: () => {
								this.handleDelete();
							}
						}
					]);
				}
			}
		},
		methods: {
			async fetchConfig() {
				this.config = await this.$api.request("get", "/config", {
					volume: this.volume
				});
			},
			async handleSubmit() {
				await this.$api.request("post", "/config", {
					volume: this.volume,
					config: this.config
				});
			},
			async handleDelete() {
				await this.$api.request("delete", "/config", {
					volume: this.volume
				});
			}
		}
	};
</script>
