<template>
	<div :class="containerClass">
		<span class="irform-array-items" @click.stop="">
			<div v-for="value, index in valueListToDisplay"
					:key="index"
					:class="{'irform-array-item': true, 'irform-array-item-inline': inline, 'irform-array-item-draggable': isDraggable(index)}"
					v-touch="getTouchConfig(index)">

				<div class="irform-array-item-draghandle" v-if="gripHandle">&nbsp;</div>

				<Form v-if="template instanceof Array"
						class="irform-array-item-body"
						:description="template"
						:disable="disable"
						:value="value"
						@input="itemUpdate(index, $event)"
						@active="handleActive">
				</Form>

				<component v-else
						class="irform-array-item-body"
						:is="template"
						:disable="disable"
						:value="value"
						:config="templateComponentConfig"
						@input="itemUpdate(index, $event)"
						@delete="itemDelete(index)">
				</component>

				<div class="irform-array-item-delete" v-if="allowDelete" @click="itemDelete(index)">x</div>
			</div>
		</span>
		<span :class="{'irform-array-add': true, 'irform-disable': !isAddEnabled}">
			<Form :description="templateAdd"
					:disable="!isAddEnabled"
					@active="handleActive">
			</Form>
		</span>
	</div>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	import Touch from "[lib]/vue/directives/touch.js"

	export default {
		mixins: [Element],
		components: {
			// Load asynchronously the form to avoid any circular dependencies
			Form: () => import("../form.vue")
		},
		directives: {
			touch: Touch
		},
		props: {
			value: {type: Array, required: false, default: () => []}
		},
		data: function() {
			return {
				/**
				 * Form description of the template of one single item
				 */
				template: this.getOption("template", [{ type: "Input", name: "text" }]),
				/**
				 * Form description of the template of one single item
				 */
				templateAdd: this.getOption("templateAdd", [{ type: "Submit", content: "Add", click: this.itemAdd }]),
				/**
				 * Force the value to have at least one entry
				 */
				atLeastOne: this.getOption("atLeastOne", false),
				/**
				 * Allow add a new element even if one of the others is empty
				 */
				allowAddIfEmpty: this.getOption("allowAddIfEmpty", false),
				/**
				 * Allow items to be deleted
				 */
				allowDelete: this.getOption("allowDelete", true),
				/**
				 * Allow the elements to be inline
				 */
				inline: this.getOption("inline", false),
				/**
				 * Display the grip handle
				 */
				gripHandle: this.getOption("gripHandle", true),
				/**
				 * Maximum number of items (if set to 0, there is no limit)
				 */
				max: this.getOption("max", 0)
			}
		},
		computed: {
			valueType() {
				return "list";
			},
			containerClass() {
				return "irform-array";
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
			 * Tells how many remaning items can be added
			 */
			nbLeft() {
				return (this.max === 0) ? Number.MAX_SAFE_INTEGER : Math.max(this.max - this.valueListToDisplay.length, 0);
			},
			/**
			 * Tells if the array is full or not
			 */
			isFull() {
				return (this.nbLeft === 0);
			},
			/**
			 * Tells if the add or create button should be active
			 */
			isAddEnabled() {
				return !(this.disable || (!this.allowAddIfEmpty && this.isAtLeastOneItemEmpty)) && !this.isFull;
			},
			valueList() {
				let list = this.get();
				return (this.atLeastOne && this.list.length == 0) ? [{}] : list;
			},
			/**
			 * The data to be displayed
			 */
			valueListToDisplay() {
				return this.valueList;
			},
			/**
			 * Pass specific options to the template if 
			 */
			templateComponentConfig() {
				return null;
			}
		},
		methods: {
			isDraggable(index) {
				return !this.disable && index < this.valueList.length;
			},
			getTouchConfig(index) {
				return {
					drag: this.isDraggable(index),
					ondrop: (dropObj) => { this.itemMove(index, dropObj.index); },
					dragElt: null,
					placeholder: "cloneNode",
					placeholderClass: "irform-drag-placeholder",
					bodyDragClass: "irform-drag-active",
					nop: true,
					allowClickThrough: true,
					triggerEvent: null
				};
			},
			itemAdd(content) {
				if (this.isAddEnabled) {
					let valueList = this.valueList.slice(0);
					valueList.push(content || {});
					this.set(valueList);
				}
			},
			itemUpdate(index, value) {
				let valueList = this.valueList.slice(0);
				valueList[index] = value;
				this.set(valueList);
			},
			itemDelete(index) {
				let valueList = this.valueList.slice(0);
				valueList.splice(index, 1);
				this.set(valueList);
			},
			itemMove(indexFrom, indexTo) {
				let valueList = this.valueList.slice(0);
				const elt = valueList.splice(indexFrom, 1)[0];
				valueList.splice(indexTo, 0, elt);
				this.set(valueList);
			}
		}
	}
</script>
