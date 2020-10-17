<script>
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
					return ["directory", "bucket"].includes(this.item.type);
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
				return this.pathList.slice(-1);
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
	};
</script>
