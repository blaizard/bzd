<script>
	import HttpClient from "#bzd/nodejs/core/http/client.mjs";

	export default {
		props: {
			pathList: { mandatory: true, type: Array },
		},
		computed: {
			itemList() {
				const containingPath = this.pathList.slice(0, -1);
				return this.$cache.getReactive("list", ...containingPath);
			},
			isExpandable() {
				if (this.item) {
					return this.item.permissions.permissions.list;
				}
				return false;
			},
			innerItemList() {
				if (this.isExpandable) {
					return this.$cache.getReactive("list", ...this.pathList);
				}
				return null;
			},
			name() {
				return this.pathList.slice(-1)[0];
			},
			item() {
				for (const item of this.itemList) {
					if (item.name == this.name) {
						return item;
					}
				}
				return null;
			},
		},
		methods: {
			/// Request the backend server.
			///
			/// This method deals with authentication if needed.
			async requestBackend(endpoint, options = {}) {
				await this.$authentication.updateFetch(options);
				return await HttpClient.request(endpoint, options);
			},
		},
	};
</script>
