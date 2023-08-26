<template>
	<div v-loading="loading">
		<h1>Configuration</h1>
		<Form :description="formDescription" v-model="config" @error="handleError"></Form>
		<Form
			:description="formDescriptionTyped || []"
			v-model="config"
			@submit="handleSubmitConfig"
			@error="handleError"
		></Form>
	</div>
</template>

<script>
	import Plugins from "../plugins/frontend.mjs";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";

	export default {
		props: {
			volume: { mandatory: false, type: String, default: null },
		},
		directives: {
			loading: DirectiveLoading,
		},
		components: {
			Form,
		},
		mixins: [Component],
		mounted() {
			if (this.volume !== null) {
				this.fetchConfig();
			}
		},
		data: function () {
			return {
				config: {},
			};
		},
		computed: {
			formDescription() {
				return [
					{
						type: "Input",
						name: "volume",
						caption: "Name",
						validation: "mandatory",
					},
					{
						type: "Dropdown",
						name: "type",
						caption: "Type",
						validation: "mandatory",
						list: Object.keys(Plugins).reduce((obj, key) => {
							obj[key] = Plugins[key].name;
							return obj;
						}, {}),
					},
				];
			},
		},
		asyncComputed: {
			async formDescriptionTyped() {
				if (this.config.type in Plugins) {
					const description = await Plugins[this.config.type].form;
					return description.concat([
						{
							type: "Button",
							action: "approve",
						},
						{
							type: "Button",
							action: "danger",
							content: "Delete",
							click: () => {
								this.handleDelete();
							},
						},
					]);
				}
			},
		},
		methods: {
			async fetchConfig() {
				await this.handleSubmit(async () => {
					this.config = await this.$api.request("get", "/config", {
						volume: this.volume,
					});
					this.config.volume = this.volume;
				});
			},
			async handleSubmitConfig() {
				await this.handleSubmit(async () => {
					await this.$api.request("post", "/config", {
						volume: this.volume,
						config: this.config,
					});
					this.$routerDispatch("/refresh");
				});
			},
			async handleDelete() {
				await this.handleSubmit(async () => {
					await this.$api.request("delete", "/config", {
						volume: this.volume,
					});
					this.$routerDispatch("/refresh");
				});
			},
		},
	};
</script>
