<template>
	<div v-loading="!ready">
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
	</div>
</template>

<script>
	import Form from "bzd/vue/components/form/form.vue";
	import Button from "bzd/vue/components/form/element/button.vue";
	import Colors from "bzd-style/css/colors.scss";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";
	import { Source, Visualization } from "../plugins/plugins.mjs";

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
		data: function () {
			return {
				ready: true,
				value: {
					"visualization.color": "auto",
				},
				pluginsVisualization: {},
				pluginsSource: {},
			};
		},
		mounted() {
			this.fetchPlugins(Source)
				.then((plugins) => {
					this.pluginsSource = plugins;
				})
				.catch(console.error);
			this.fetchPlugins(Visualization)
				.then((plugins) => {
					this.pluginsVisualization = plugins;
				})
				.catch(console.error);

			// Update specific uid if needed
			if (this.isUpdate) {
				this.ready = false;
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
						list: this.dropdownPluginList(this.pluginsVisualization, "*"),
						html: true,
					},
				].concat(...(this.metadataVisualization["form"] || []));
			},
			metadataSource() {
				return this.pluginsSource[this.value["source.type"]] || {};
			},
			formPluginSourceDescription() {
				return this.metadataSource.form || [];
			},
			metadataVisualization() {
				return this.pluginsVisualization[this.value["visualization.type"]] || {};
			},
			dropdownSourceList() {
				return this.dropdownPluginList(this.pluginsSource, this.value["visualization.type"]);
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
						}
					);
			},
		},
		methods: {
			async fetchValue() {
				this.value = Object.assign({}, this.value, await this.$api.request("get", "/tile", { uid: this.uid }));
				this.ready = true;
			},
			async handleCreate() {
				await this.$api.request("post", "/tile", this.value);
			},
			async handleUpdate() {
				await this.$api.request("put", "/tile", { uid: this.uid, value: this.value });
			},
			async fetchPlugins(source) {
				let plugins = {};
				for (const [name, description] of Object.entries(source)) {
					if ("frontend" in description) {
						await description.frontend(); // Load the frontend plugin to load the icon
					}
					plugins[name] = Object.assign({}, description);
				}
				return plugins;
			},
			dropdownPluginList(plugins, filter) {
				const list = Object.keys(plugins)
					.filter((type) => {
						if (filter == "*") {
							return true;
						}
						return (plugins[type].visualization || []).includes(filter);
					})
					.map((type) => {
						return {
							key: type,
							html: "<i class=\"" + plugins[type].icon + "\"></i> " + decodeURIComponent(plugins[type].name || type),
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
