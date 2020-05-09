<template>
	<div class="irform-date">
		<ElementInput
				:description="inputDescription"
				:value="value">
		</ElementInput>
		<div>
            <div class="irform-date-header">
                <div v-show="Number(get()) >= nbDaysPrevMonth * 24 * 60 * 60 * 1000"
                        class="irform-date-previous"
                        @click="set(Number(get()) - nbDaysPrevMonth * 24 * 60 * 60 * 1000)">
                </div>
                <div class="irform-date-month">{{ this.months[date.getMonth()] }} {{ date.getFullYear() }}</div>
                <div class="irform-date-next"
                        @click="set(Number(get()) + nbDaysCurMonth * 24 * 60 * 60 * 1000)">
                </div>
            </div>
            <div class="irform-date-row">
                <div v-for="text in days" :class="{'irform-date-item': true, 'irform-date-item-header': true}">{{ text }}</div>
            </div>
            <div v-for="row in daysMatrix" class="irform-date-row">
                <div v-for="day in row"
                        :class="{'irform-date-item': true, 'irform-date-item-current': day.current, 'irform-date-item-selected': day.selected}"
                        @click="selectDate(day.date || null)">
                    {{ day.number }}
                </div>
            </div>
		</div>
	</div>
</template>

<script>
"use strict";

import Element from "./element.vue";
import ElementInput from "./input.vue";

export default {
	mixins: [Element],
	components: {
		ElementInput
	},
	props: {
		value: {type: Number, required: false, default: 0}
	},
	data: function() {
		return {
			/**
                    * If the displayed values should be interpreted as HTML or not
                    */
			format: this.getOption("format", "{year}/{month}/{day}"),
			/**
                 * Week days, first day is sunday.
                 */
			days: this.getOption("days", ["S", "M", "T", "W", "T", "F", "S"]),
			/**
                 * Months.
                 */
			months: this.getOption("months", ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]),
		};
	},
	computed: {
		date() {
			const date = new Date(this.get());
			return (date instanceof Date && !isNaN(date)) ? date : new Date();
		},
		inputDescription() {
			return Object.assign({}, this.description, {
				presets: {
					//     [this.value]: String(this.date) 
				}
			});
		},
		nbDaysCurMonth() {
			return new Date(this.date.getFullYear(), this.date.getMonth() + 1, 0).getDate();
		},
		nbDaysPrevMonth() {
			return new Date(this.date.getFullYear(), this.date.getMonth(), 0).getDate();
		},
		daysMatrix() {
			const firstDay = new Date(this.date.getFullYear(), this.date.getMonth(), 1).getDay();

			let matrix = [];
			let rowIndex = -1;
			let day = 0;
			let index = firstDay;
			while (day < this.nbDaysCurMonth) {
				matrix[++rowIndex] = [];
				for (; index<7 && day < this.nbDaysCurMonth; ++index) {
					++day;
					matrix[rowIndex].push({current: true, selected: (day == this.date.getDate()), number: day, date: new Date(this.date.getFullYear(), this.date.getMonth(), day)});
				}
				index = 0;
			}

			for (let i = 0; i<firstDay; ++i) {
				matrix[0].unshift({current: false, selected: false, number: this.nbDaysPrevMonth - i});
			}
			for (let i = 1; matrix[rowIndex].length < 7; ++i) {
				matrix[rowIndex].push({current: false, selected: false, number: i});
			}

			return matrix;
		}
	},
	methods: {
		selectDate(date) {
			if (date === null) {
				return;
			}
			this.set(date.getTime());
		}
	}
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/date.scss";
</style>
