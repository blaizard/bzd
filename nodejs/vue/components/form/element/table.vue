<template>
	<div :class="containerClass">
		<table class="irform-table-container">
			<thead>
				<tr class="irform-table-headers">
					<th
						v-for="(description, index) in template"
						v-if="isConditionSatisfied(description)"
						:key="index"
						:class="getHeaderClass(description)"
						@click="handleHeaderClick(description)"
					>
						{{ description.caption }}
					</th>
				</tr>
			</thead>
			<tbody>
				<Form
					v-for="(row, index) in rowListToDisplay"
					class="irform-table-row"
					tag="tr"
					:key="row.index"
					:description="template"
					:class="rowClass(row.value, index)"
					:disable="row.disable"
					:model-value="row.value"
					:template="itemTemplate"
					:context="{ row: row.index }"
					@update-with-context="itemUpdate(row.index, $event)"
					@active="handleActive"
				>
				</Form>
			</tbody>
		</table>
	</div>
</template>

<script>
	import Element from "./element.vue";
	import TableItem from "./table_item.vue";
	import { defineAsyncComponent } from "vue";

	export default {
		mixins: [Element],
		components: {
			// Load asynchronously the form to avoid any circular dependencies
			Form: defineAsyncComponent(() => import("../form.vue")),
		},
		data: function () {
			return {
				/// Sort the table by the columns name
				sort: this.getOption("sort", []),
			};
		},
		computed: {
			/// ---- CONFIG ----------------------------------------
			/// Return the class associated with a specific row
			rowClass() {
				return this.getOption("rowClass", (/*value, index*/) => "");
			},
			/// Return whether or not a row should be disabled
			rowDisable() {
				return this.getOption("rowDisable", (/*value*/) => false);
			},
			/// The columns to be displayed
			template() {
				return this.getOption("template", []).map((item, index) => {
					// If there is no name associated with the column, create one
					if (!("name" in item)) {
						item.name = "col-" + index;
					}
					return item;
				});
			},
			/// ---- IMPLEMENTATION ----------------------------------
			containerClass() {
				return {
					"irform-table": true,
					[this.getOption("class")]: true,
				};
			},
			valueType() {
				return "list";
			},
			templateObj() {
				return this.template.reduce((obj, template) => {
					if ("name" in template) {
						obj[template.name] = template;
					}
					return obj;
				}, {});
			},
			rowList() {
				return this.get().map((value, index) => ({
					index: index,
					value: value,
					disable: this.rowDisable(value) || this.disable,
				}));
			},
			sortingList() {
				return this.sort.map((sort) => {
					let key = sort;
					let compare = true;
					if (sort instanceof Array) {
						key = sort[0];
						compare = typeof sort[1] === "undefined" ? true : sort[1];
					}
					return {
						compare:
							typeof compare === "function"
								? compare
								: (a, b) => {
										const valueA = a.value[key];
										const valueB = b.value[key];

										let result = 0;
										switch ((this.templateObj[key] || {}).valueType) {
											case "number": {
												const numberA = parseFloat(valueA);
												const numberB = parseFloat(valueB);
												result =
													(isNaN(numberA) ? Number.NEGATIVE_INFINITY : numberA) -
													(isNaN(numberB) ? Number.NEGATIVE_INFINITY : numberB);
												break;
											}

											default:
												result = String(valueA || "").localeCompare(String(valueB || ""));
										}

										return compare ? result : -result;
									},
						key: key,
						order: typeof compare === "function" ? "unknown" : compare ? "descending" : "ascending",
					};
				});
			},
			sortingObj() {
				return this.sortingList.reduce((obj, sort) => {
					obj[sort.key] = sort;
					return obj;
				}, {});
			},
			rowListToDisplay() {
				return this.rowList.sort((a, b) => {
					let result = 0;
					for (const i in this.sortingList) {
						const sort = this.sortingList[i];
						result = sort.compare(a, b);
						// If equality, go to the next
						if (result !== 0) {
							break;
						}
					}
					return result;
				});
			},
			itemTemplate() {
				return TableItem;
			},
		},
		methods: {
			isConditionSatisfied(description) {
				if (typeof description.condition === "function") {
					return description.condition({});
				}
				return typeof description.condition === "undefined" ? true : description.condition;
			},
			getHeaderClass(description) {
				const sort = this.sortingObj[description.name];
				return {
					"irform-table-header": true,
					"irform-table-header-order-unknown": sort && sort.order == "unknown",
					"irform-table-header-order-ascending": sort && sort.order == "ascending",
					"irform-table-header-order-descending": sort && sort.order == "descending",
				};
			},
			handleHeaderClick(description) {
				let direction = true;
				if (description.name in this.sortingObj) {
					direction = this.sortingObj[description.name].order !== "descending";
				}
				this.sort = [[description.name, direction]];
			},
			itemUpdate(rowIndex, data) {
				let valueList = this.get().slice(0);
				valueList[rowIndex] = data.value;
				this.set(valueList, { ...data.context, row: rowIndex, action: "update" });
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/styles/default/css/form/table.scss" as *;
</style>
