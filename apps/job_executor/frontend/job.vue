<template>
	<div>
		<h1>Job {{ job }}</h1>
		<TerminalWebsocket :websocket-endpoint="websocketEndpoint" style="height: 600px"></TerminalWebsocket>
		<h2>Files</h2>
		<FileExplorer class="files" :fetch="fetchPath" :download="download" :metadata="true"></FileExplorer>
	</div>
</template>

<script>
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import TerminalWebsocket from "#bzd/nodejs/vue/components/terminal/frontend/websocket.vue";
	import FileExplorer from "#bzd/nodejs/vue/components/file_explorer/file_explorer.vue";
	import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";

	const Exception = ExceptionFactory("main");

	export default {
		mixins: [Component],
		components: {
			TerminalWebsocket,
			FileExplorer,
		},
		props: {
			job: { type: String, required: true },
		},
		computed: {
			websocketEndpoint() {
				return "/job/" + this.job;
			},
		},
		data: function () {
			return {
				value: {},
			};
		},
		methods: {
			async fetchPath(pathList) {
				let next = 1000;
				let list = [];
				do {
					const response = await this.$rest.request("post", "/files/{id}", {
						id: this.job,
						path: pathList,
						paging: next,
					});
					next = response.next;
					list = list.concat(
						response.data.map((item) => {
							item.permissions = Permissions.makeFromEntry(item);
							return item;
						}),
					);
				} while (next);
				return list;
			},
			download(pathList) {
				window.location =
					this.$rest.getEndpoint("/file/" + this.job) + "?path=" + encodeURIComponent(pathList.join("/"));
			},
		},
	};
</script>

<style lang="scss" scoped>
	.files {
		border: 1px solid #000;
		padding: 10px 10px 10px 20px;
	}
</style>
