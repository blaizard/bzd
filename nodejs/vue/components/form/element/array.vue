<template>
	<div :class="containerClass">
		<span v-if="valueListToDisplay.length" class="irform-array-items" @click.stop="">
			<div
				v-for="(value, index) in valueListToDisplay"
				:key="index"
				:class="{
					'irform-array-item': true,
					'irform-array-item-inline': inline,
					'irform-array-item-draggable': isDraggable(index),
				}"
				v-touch="getTouchConfig(index)"
				@click.stop="itemClick(index)"
			>
				<div class="irform-array-item-draghandle" v-if="gripHandle">&nbsp;</div>

				<Form
					class="irform-array-item-body"
					:description="template"
					:disable="disable"
					:model-value="value"
					:context="{ row: index }"
					@update-with-context="itemUpdate(index, $event)"
					@active="handleActive"
				>
				</Form>

				<div class="irform-array-item-delete" v-if="allowDelete" @click="itemDelete(index)">x</div>
			</div>
		</span>

		<span
			v-if="allowAdd && (isAddEnabled || !hideAddWhenDisabled)"
			:class="{ 'irform-array-add': true, 'irform-disable': !isAddEnabled }"
		>
			<Form :description="templateAdd" :disable="!isAddEnabled" @active="handleActive" @submit="itemAdd"> </Form>
		</span>
	</div>
</template>

<script>
	import Element from "./element.vue";
	import Touch from "../../../directives/touch.mjs";
	import { defineAsyncComponent } from "vue";

	export default {
		mixins: [Element],
		components: {
			// Load asynchronously the form to avoid any circular dependencies
			Form: defineAsyncComponent(() => import("../form.vue")),
		},
		directives: {
			touch: Touch,
		},
		computed: {
			/// ---- CONFIG ----------------------------------------
			/// Form description of the template of one single item
			template() {
				return this.getOption("template", [{ type: "Input", name: "text" }]);
			},
			/// Form description of the template of one single item
			templateAdd() {
				return this.getOption("templateAdd", [{ type: "Button", content: "Add", action: "approve" }]);
			},
			/// Conversion function to translate to values when add is pressed.
			toValues() {
				return this.getOption("toValues", (values) => values);
			},
			/// Force the value to have at least one entry
			atLeastOne() {
				return this.getOption("atLeastOne", false);
			},
			/// Allow add a new element
			allowAdd() {
				return this.getOption("allowAdd", true);
			},
			/// Allow add a new element even if one of the others is empty
			allowAddIfEmpty() {
				return this.getOption("allowAddIfEmpty", false);
			},
			/// Allow items to be deleted
			allowDelete() {
				return this.getOption("allowDelete", true);
			},
			/// Allow the elements to be inline
			inline() {
				return this.getOption("inline", false);
			},
			/// Display the grip handle
			gripHandle() {
				return this.getOption("gripHandle", true);
			},
			/// Hide the add button when it is supposed to be disabled
			hideAddWhenDisabled() {
				return this.getOption("hideAddWhenDisabled", false);
			},
			/// Maximum number of items (if set to 0, there is no limit)
			max() {
				return this.getOption("max", 0);
			},
			/// Let the component fill the whole available space,
			/// this should be only used when max = 1
			fill() {
				return this.getOption("fill", false);
			},
			/// ---- IMPLEMENTATION ----------------------------------
			valueType() {
				return "list";
			},
			containerClass() {
				return {
					"irform-array": true,
					"irform-fill": this.fill,
					[this.getOption("class")]: true,
				};
			},
			/**
			 * Returns true if an element in the list is empty
			 */
			isAtLeastOneItemEmpty() {
				return this.valueList.some((value) => {
					if (typeof value === "object") {
						return Object.keys(value).length == 0;
					}
					return !value;
				});
			},
			/**
			 * Tells how many remaining items can be added
			 */
			nbLeft() {
				return this.max === 0 ? Number.MAX_SAFE_INTEGER : Math.max(this.max - this.valueListToDisplay.length, 0);
			},
			/**
			 * Tells if the array is full or not
			 */
			isFull() {
				return this.nbLeft === 0;
			},
			/**
			 * Tells if the add or create button should be active
			 */
			isAddEnabled() {
				return !(this.disable || (!this.allowAddIfEmpty && this.isAtLeastOneItemEmpty)) && !this.isFull;
			},
			valueList() {
				let list = this.get();
				return this.atLeastOne && this.list.length == 0 ? [{}] : list;
			},
			/**
			 * The data to be displayed
			 */
			valueListToDisplay() {
				return this.valueList;
			},
			/**
			 * Pass specific options to the template
			 */
			templateComponentConfig() {
				return null;
			},
		},
		methods: {
			isDraggable(index) {
				return !this.disable && this.valueList.length > 1 && index < this.valueList.length;
			},
			getTouchConfig(index) {
				return {
					drag: this.isDraggable(index),
					ondrop: (dropObj) => {
						this.itemMove(index, dropObj.index);
					},
					dragElt: null,
					placeholder: "cloneNode",
					placeholderClass: "irform-drag-placeholder",
					bodyDragClass: "irform-drag-active",
					nop: true,
					allowClickThrough: true,
					triggerEvent: null,
				};
			},
			itemClick(/*index*/) {},
			async itemAdd(content) {
				if (this.isAddEnabled) {
					let valueList = this.valueList.slice(0);
					const value = this.toValues(content || {});
					valueList.push(value);
					await this.set(valueList, { action: "add", value: value });
				}
			},
			async itemUpdate(index, data) {
				let valueList = this.valueList.slice(0);
				valueList[index] = data.value;
				await this.set(valueList, { ...data.context, row: index, action: "update" });
			},
			async itemDelete(index) {
				let valueList = this.valueList.slice(0);
				const value = valueList.pop(index);
				await this.set(valueList, { action: "delete", row: index, value: value });
			},
			async itemMove(indexFrom, indexTo) {
				let valueList = this.valueList.slice(0);
				const elt = valueList.splice(indexFrom, 1)[0];
				valueList.splice(indexTo, 0, elt);
				await this.set(valueList, { action: "move", rowFrom: indexFrom, rowTo: indexTo, value: elt });
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/array.scss" as *;
</style>
