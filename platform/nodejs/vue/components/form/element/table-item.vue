<template>
	<td :class="cellClass" @click="selectItem">
		<slot :setError="setError"></slot>
	</td>
</template>

<script>
	"use strict";

	import Item from "../item.vue"

	export default {
		mixins: [Item],
		props: {
			active: {type: Boolean, required: false, default: false},
			disable: {type: Boolean, default: false, required: false}
		},
		computed: {
			cellClass() {
				return {
					"irform-table-cell": true,
					"irform-disable": this.disable,
					"irform-error": this.isError,
					"irform-active": this.active
				}
			}
		},
		methods: {
			selectItem() {
				const item = this.$el.querySelector("*[tabindex]");
				if (item) {
					item.focus();
				}
			}
		}
	}
</script>
