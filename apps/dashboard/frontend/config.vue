<template>
	<div v-loading="loading">
		<h2>Config</h2>
		<Form :description="formDescription" v-model="value"></Form>
		<Form :description="formVisualizationDescription" v-model="value"></Form>

		<template v-if="isSources">
			<h3>Source(s)</h3>
			<Form :description="formSourceDescription" v-model="value"></Form>
			<Form :description="formPluginSourceDescription" v-model="value"></Form>
		</template>

		<br />
		<Button v-if="isUpdate" @click="handleUpdate" action="approve" content="Update"></Button>
		<Button v-else @click="handleCreate" action="approve" content="Create"></Button>
		<Button @click="handleDelete" action="danger" content="Delete"></Button>
	</div>
</template>

<script>
	import Form from "bzd/vue/components/form/form.vue";
	import Button from "bzd/vue/components/form/element/button.vue";
	import Colors from "bzd-style/css/colors.scss";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";
	import Plugins from "../plugins/plugins.frontend.index.mjs";
	import ExceptionFactory from "bzd/core/exception.mjs";
	import Component from "bzd/vue/components/layout/component.vue";

	const Exception = ExceptionFactory("config");

	export default {
		components: {
			Form,
			Button,
		},
		directives: {
			loading: DirectiveLoading,
		},
		props: {
			uid: { type: String, mandatory: false, default: null },
		},
		mixins: [Component],
		data: function () {
			return {
				value: {
					"visualization.color": "auto",
				},
				plugins: {
					visualization: {},
					source: {},
				},
			};
		},
		mounted() {
			this.fetchPlugins()
				.then((plugins) => {
					this.plugins = plugins;
				})
				.catch(console.error);

			// Update specific uid if needed
			if (this.isUpdate) {
				this.fetchValue();
			}
		},
		computed: {
			isUpdate() {
				return this.uid != null;
			},
			isSources() {
				return Object.keys(this.dropdownSourceList).length > 0;
			},
			formDescription() {
				return [
					{ type: "Input", name: "name", caption: "Name", placeholder: "Enter a name...", width: 0.5 },
					{
						type: "Dropdown",
						name: "visualization.color",
						caption: "Color",
						width: 0.5,
						list: this.dropdownColorList,
						html: true,
					},
				];
			},
			formSourceDescription() {
				return [
					{
						type: "Dropdown",
						name: "source.type",
						caption: "Type",
						width: 0.5,
						list: this.dropdownSourceList,
						html: true,
						onchange: () => {
							this.value = Object.assign({}, this.value, this.metadataSource.defaultValue);
						},
					},
				];
			},
			formVisualizationDescription() {
				return [
					{
						type: "Dropdown",
						name: "visualization.type",
						caption: "Type",
						width: 0.5,
						list: this.dropdownPluginList(this.plugins.visualization, "*"),
						html: true,
					},
				].concat(...(this.metadataVisualization["form"] || []));
			},
			metadataSource() {
				return (this.plugins.source[this.value["source.type"]] || { metadata: {} }).metadata;
			},
			formPluginSourceDescription() {
				return this.metadataSource.form || [];
			},
			metadataVisualization() {
				return (this.plugins.visualization[this.value["visualization.type"]] || { metadata: {} }).metadata;
			},
			dropdownSourceList() {
				return this.dropdownPluginList(this.plugins.source, this.value["visualization.type"]);
			},
			dropdownColorList() {
				return Object.keys(Colors)
					.map((name) => {
						return [
							name,
							"<span class=\"bzd-dashboard-color-picker\" style=\"background-color: " +
								Colors[name] +
								"; border-color: " +
								Colors.black +
								";\"></span> " +
								name,
						];
					})
					.reduce(
						(acc, value) => {
							acc[value[0]] = value[1];
							return acc;
						},
						{
							auto: "Auto",
						},
					);
			},
		},
		methods: {
			async fetchValue() {
				this.handleSubmit(async () => {
					this.value = Object.assign({}, this.value, await this.$api.request("get", "/tile", { uid: this.uid }));
				});
			},
			async handleCreate() {
				this.handleSubmit(async () => {
					await this.$api.request("post", "/tile", { value: this.value });
					this.$notification.success("New tile created");
					await this.$routerDispatch("/");
				});
			},
			async handleUpdate() {
				this.handleSubmit(async () => {
					await this.$api.request("put", "/tile", { uid: this.uid, value: this.value });
					this.$notification.success("Tile updated");
					await this.$routerDispatch("/");
				});
			},
			async handleDelete() {
				this.handleSubmit(async () => {
					await this.$api.request("delete", "/tile", { uid: this.uid });
					this.$notification.success("Tile deleted");
					await this.$routerDispatch("/");
				});
			},
			/**
			 * Plugins will have the following structure:
			 * source:
			 *   travisci:
			 *      metadata:
			 *      module:
			 *   ...
			 */
			async fetchPlugins() {
				let plugins = {
					source: {},
					visualization: {},
				};
				for (const [name, data] of Object.entries(Plugins)) {
					Exception.assert("metadata" in data && "type" in data.metadata, "Missing type for plugin: '{}'", name);
					Exception.assert(
						["source", "visualization"].includes(data.metadata.type),
						"Unsupported plugin type: '{}'",
						data.metadata.type,
					);
					data.module = "module" in data ? await data.module() : {}; // Load the frontend plugin to load the icon
					plugins[data.metadata.type][name] = data;
				}

				return plugins;
			},
			dropdownPluginList(plugins, filter) {
				const list = Object.keys(plugins)
					.filter((type) => {
						if (filter == "*") {
							return true;
						}
						return (plugins[type].metadata.visualization || []).includes(filter);
					})
					.map((type) => {
						return {
							key: type,
							html:
								"<i class=\"" +
								plugins[type].metadata.icon +
								"\"></i> " +
								decodeURIComponent(plugins[type].metadata.name || type),
						};
					});

				return list.reduce((result, data) => {
					result[data.key] = data.html;
					return result;
				}, {});
			},
		},
	};
</script>

<style lang="scss">
	.bzd-dashboard-color-picker {
		display: inline-block;
		height: 1em;
		width: 1em;
		vertical-align: middle;
		border-width: 1px;
		border-style: solid;
	}
</style>
