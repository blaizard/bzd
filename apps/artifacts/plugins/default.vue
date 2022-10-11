<template>
	<div>
		<h2>Metadata</h2>
		<div class="metadata">
			<div v-for="key in Object.keys(item || {}).sort(sortStrategy)" class="metadata-entry" :key="key">
				<span class="metadata-entry-key">{{ key }}</span>
				<span class="metadata-entry-value"><code>{{ displayValue(key, item[key]) }}</code></span>
			</div>
		</div>
		<template v-if="isActions">
			<h2>Actions</h2>
			<a v-if="isDownload(item)" :href="downloadLink()" :download="name"><Button action="approve" content="Download"></Button></a>
		</template>
		<template v-if="innerItemList">
			<h2>
				Contains
				<small>({{ innerItemList.length }}
					<template v-if="innerItemList.length > 1">items)</template>
					<template v-else>item)</template></small>
			</h2>
			<table>
				<tr>
					<th v-for="key in innerItemListSortedKeys" :key="key">{{ key }}</th>
					<th></th>
				</tr>
				<tr v-for="(item, index) in innerItemList" :key="index">
					<td v-for="key in innerItemListSortedKeys" :key="key">
						{{ key in item ? displayValue(key, item[key]) : "" }}
					</td>
					<td class="actions">
						<a
							v-if="isDownload(item)"
							class="bzd-icon-upload"
							:href="downloadLink(item)"
							:download="item.name"
							v-tooltip="tooltipActionDownload"></a>
					</td>
				</tr>
			</table>
		</template>
	</div>
</template>

<script>
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";
	import Base from "./base.vue";
	import Button from "bzd/vue/components/form/element/button.vue";
	import { bytesToString } from "bzd/utils/to_string.mjs";

	export default {
		mixins: [Base],
		components: {
			Button,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		computed: {
			innerItemListSortedKeys() {
				let keys = new Set();
				for (const item of this.innerItemList) {
					Object.keys(item).forEach((key) => {
						keys.add(key);
					});
				}
				const keyList = Array.from(keys);
				return keyList.sort(this.sortStrategy);
			},
			isActions() {
				return this.item && this.isDownload(this.item);
			},
			tooltipActionDownload() {
				return {
					type: "text",
					data: "Download",
				};
			},
		},
		methods: {
			isDownload(item) {
				return item.permissions.isRead() && !item.permissions.isList();
			},
			downloadLink(item) {
				const pathList = item ? [...this.pathList, item.name] : this.pathList;
				return "/file/" + pathList.map(encodeURIComponent).join("/");
			},
			sortStrategy(key1, key2) {
				const firsts = ["name", "type", "size", "created", "modified"];
				const pos1 = firsts.indexOf(key1);
				const pos2 = firsts.indexOf(key2);
				if (pos1 == -1 && pos2 == -1) {
					return key1.localeCompare(key2);
				}
				if (pos1 == -1) {
					return 1;
				}
				if (pos2 == -1) {
					return -1;
				}
				return pos1 < pos2;
			},
			displayValue(key, value) {
				switch (key) {
				case "size":
					return bytesToString(value);
				default:
					return value;
				}
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" as icons with (
		$bzdIconNames: upload
	);
</style>

<style lang="scss" scoped>
	.metadata {
		columns: 4 200px;
		column-gap: 4em;
		.metadata-entry {
			.metadata-entry-key {
				&:after {
					content: ":";
				}
			}
			.metadata-entry-value {
				code {
					white-space: normal !important;
				}
			}
		}
	}
	.actions {
		width: 0;
	}
</style>
