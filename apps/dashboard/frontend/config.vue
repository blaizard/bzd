<template>
	<div>
        <h2>Config</h2>
        <Form :description="formDescription" v-model="value"></Form>
        <Form :description="formPluginSourceDescription" v-model="value"></Form>
        <h3>Visualization</h3>
        <Form :description="formVisualizationDescription" v-model="value"></Form>
        <Form :description="formPluginVisualizationDescription" v-model="value"></Form>
        <h3>Mapping</h3>
        {{ value }}
        <button @click="handleCreate">Create</button>
    </div>
</template>

<script>
	"use strict"

    import Form from "[bzd]/vue/components/form/form.vue";
    import { Source, Visualization } from "[dashboard]/plugins/plugins.js";

	export default {
        components: {
            Form
        },
		data: function () {
			return {
                value: {},
                pluginsSource: {},
                pluginsVisualization: {}
			}
		},
        mounted() {
            this.fetchPlugins(Source).then((plugins) => { this.pluginsSource = plugins; }).catch(console.error);
            this.fetchPlugins(Visualization).then((plugins) => { this.pluginsVisualization = plugins; }).catch(console.error);
        },
        computed: {
            formDescription() {
                return [
					{ type: "Input", name: "name", caption: "Name", placeholder: "Enter a name...", width: 0.5 },
					{ type: "Dropdown", name: "source.type", caption: "Type", width: 0.5, list: this.dropdownSourceList, html: true, onchange: (type) => {
                        for (const [key, value] of Object.entries(this.metadataSource.defaultValue)) {
                            this.$set(this.value, key, value);
                        }
                    }},
				]
            },
            formVisualizationDescription() {
                return [
					{ type: "Dropdown", name: "visualization.type", caption: "Type", width: 0.5, list: this.dropdownVisualizationList, html: true },
				]
            },
            metadataSource() {
                return this.getMetadata(this.pluginsSource, this.value["source.type"]);
            },
            formPluginSourceDescription() {
                return this.metadataSource["form"] || [];
            },
            metadataVisualization() {
                return this.getMetadata(this.pluginsVisualization, this.value["visualization.type"]);
            },
            formPluginVisualizationDescription() {
                return this.metadataVisualization["form"] || [];
            },
            dropdownSourceList() {
                return this.dropdownPluginList(this.pluginsSource);
            },
            dropdownVisualizationList() {
                return this.dropdownPluginList(this.pluginsVisualization);
            }
        },
        methods: {
            getMetadata(plugins, type) {
                return (type in plugins) ? plugins[type].methods.getMetadata() : {};
            },
            async handleCreate() {
                await this.$api.request("post", "/tile", { data: this.value });
            },
            async fetchPlugins(source) {
                let plugins = {};
                for (const [name, description] of Object.entries(source)) {
                    plugins[name] = (await description.frontend()).default;
                }
                return plugins;
            },
            dropdownPluginList(plugins) {
                const list = Object.keys(plugins).map((type) => {
                    const metadata = this.getMetadata(plugins, type);
                    return {key: type, html: "<i class=\"" + metadata.icon + "\"></i> " + decodeURIComponent(metadata.name || type)};
                });

                return list.reduce((result, data) => {
                    result[data.key] = data.html;
                    return result;
                }, {});
            } 
        }
	}
</script>
