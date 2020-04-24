<template>
	<div>
        <h2>Config</h2>
        <Form :description="formDescription" v-model="value"></Form>
        <Form :description="formPluginDescription" v-model="value"></Form>
        {{ value }}
        <button @click="handleCreate">Create</button>
    </div>
</template>

<script>
	"use strict"

    import Form from "[bzd]/vue/components/form/form.vue";
    import { Frontend } from "[dashboard]/plugins/plugins.js";

	export default {
        components: {
            Form
        },
		data: function () {
			return {
                value: {},
                plugins: {}
			}
		},
        mounted() {
            this.fetchPlugins();
        },
        computed: {
            formDescription() {
                return [
					{ type: "Input", name: "name", caption: "Name", placeholder: "Enter a name...", width: 0.5 },
					{ type: "Dropdown", name: "type", caption: "Type", width: 0.5, list: this.fetchPlugins, html: true },
				]
            },
            metadata() {
                return this.getMetadata(this.value.type);
            },
            formPluginDescription() {
                return this.metadata["form"] || [];
            }
        },
        methods: {
            getMetadata(type) {
                return (type in this.plugins) ? this.plugins[type].methods.getMetadata() : {};
            },
            async handleCreate() {
                await this.$api.request("post", "/tile", { data: this.value });
            },
            async fetchPlugins() {
                let plugins = {};
                for (const [name, loader] of Object.entries(Frontend)) {
                    this.$set(this.plugins, name, (await loader()).default);
                }

                const list = Object.keys(this.plugins).map((type) => {
                    const metadata = this.getMetadata(type);
                    return {key: type, html: "<i class=\"" + metadata.icon + "\"></i> " + decodeURIComponent(type)};
                });

                return list.reduce((result, data) => {
                    result[data.key] = data.html;
                    return result;
                }, {});
            }
        }
	}
</script>
