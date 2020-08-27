<template>
	<div>
		<h2>Metadata</h2>
		<div class="metadata">
			<div v-for="key in Object.keys(item || {}).sort(sortStrategy)" class="metadata-entry" :key="key">
				<span class="metadata-entry-key">{{ key }}</span>
				<span class="metadata-entry-value"><code>{{ displayValue(key, item[key]) }}</code></span>
			</div>
		</div>
		<template v-if="innerItemList">
			<h2>Contains</h2>
			<table>
				<tr>
					<th v-for="key in innerItemListSortedKeys" :key="key">{{ key }}</th>
				</tr>
				<tr v-for="(item, index) in innerItemList" :key="index">
					<td v-for="key in innerItemListSortedKeys" :key="key">
						{{ key in item ? displayValue(key, item[key]) : "" }}
					</td>
				</tr>
			</table>
		</template>
	</div>
</template>

<script>
	import Base from "./base.vue";

	import { bytesToString } from "bzd/utils/to_string.mjs";

	export default {
		mixins: [Base],
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
			}
		},
		methods: {
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
			}
		}
	};
</script>

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
</style>
