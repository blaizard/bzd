<template>
	<DropdownTemplate
		:class="containerClass"
		ref="dropdown"
		:model-value="get()"
		@update:model-value="set"
		@key="handleKey"
		:disable="disable"
		:description="descriptionDropdown"
	>
		<div class="irform-date-header" @mousedown.stop.prevent="" @mouseup.stop.prevent="">
			<div class="irform-date-previous" @click.stop="set(Number(get()) - decrementStep)"></div>
			<div class="irform-date-select">
				<div class="irform-date-select-month" @click="select = 'month'">{{ currentMonthStr }}</div>
				<div class="irform-date-select-year" @click="select = 'year'">{{ currentYearStr }}</div>
			</div>
			<div class="irform-date-next" @click.stop.prevent="set(Number(get()) + incrementStep)"></div>
		</div>

		<!-- Days //-->
		<template v-if="select == 'day'">
			<div class="irform-date-row">
				<div v-for="day in weekdays" :class="{ 'irform-date-item': true, 'irform-date-item-header': true }">
					{{ day }}
				</div>
			</div>
			<div v-for="row in daysMatrix" class="irform-date-row">
				<div
					v-for="day in row"
					:class="{
						'irform-date-item': true,
						'irform-date-item-current': day.current,
						'irform-date-item-selected': day.selected,
					}"
					@mousedown.stop="selectDate($event, day.date || null)"
				>
					{{ day.text }}
				</div>
			</div>
		</template>

		<!-- Months //-->
		<template v-else-if="select == 'month'">
			<div v-for="row in monthsMatrix" class="irform-date-row">
				<div
					v-for="month in row"
					:class="{
						'irform-date-item': true,
						'irform-date-item-current': month.current,
						'irform-date-item-selected': month.selected,
					}"
					@mousedown.stop="selectDate($event, month.date || null, 'day')"
				>
					{{ month.text }}
				</div>
			</div>
		</template>

		<!-- Months //-->
		<template v-else>
			<div v-for="row in yearsMatrix" class="irform-date-row">
				<div
					v-for="month in row"
					:class="{
						'irform-date-item': true,
						'irform-date-item-current': month.current,
						'irform-date-item-selected': month.selected,
					}"
					@mousedown.stop="selectDate($event, month.date || null, 'month')"
				>
					{{ month.text }}
				</div>
			</div>
		</template>
	</DropdownTemplate>
</template>

<script>
	import Element from "./element.vue";
	import DropdownTemplate from "./dropdown_template.vue";

	export default {
		mixins: [Element],
		components: {
			DropdownTemplate,
		},
		data: function () {
			return {
				select: "day",
			};
		},
		computed: {
			valueType() {
				return "number";
			},
			containerClass() {
				return {
					"irform-date": true,
					[this.getOption("class")]: true,
				};
			},
			incrementStep() {
				switch (this.select) {
					case "day":
						return this.nbDaysCurMonth * 24 * 60 * 60 * 1000;
					case "month":
						return 365.25 * 24 * 60 * 60 * 1000;
					case "year":
						return 9 * 365.25 * 24 * 60 * 60 * 1000;
				}
				return null;
			},
			decrementStep() {
				switch (this.select) {
					case "day":
						return this.nbDaysPrevMonth * 24 * 60 * 60 * 1000;
				}
				return this.incrementStep;
			},
			weekdays() {
				const sunday = new Date(1985, 3, 7);
				const formatter = new Intl.DateTimeFormat(undefined, { weekday: "narrow" });
				return [...Array(7).keys()].map((index) =>
					formatter.format(new Date(sunday.getTime()).setDate(sunday.getDate() + index)),
				);
			},
			months() {
				const formatter = new Intl.DateTimeFormat(undefined, { month: "long" });
				return [...Array(12).keys()].map((index) => formatter.format(new Date().setMonth(index)));
			},
			date() {
				const date = new Date(this.get());
				return this.isValidDate(date) ? date : new Date();
			},
			currentMonthStr() {
				return new Intl.DateTimeFormat(undefined, { month: "long" }).format(this.date);
			},
			currentYearStr() {
				return new Intl.DateTimeFormat(undefined, { year: "numeric" }).format(this.date);
			},
			descriptionDropdown() {
				return Object.assign({}, this.description, {
					format: (value) => {
						const date = new Date(parseInt(value));
						if (!this.isValidDate(date)) {
							return "";
						}

						const options = { year: "numeric", month: "long", day: "numeric" };
						return date.toLocaleDateString(undefined, options);
					},
					valueType: "number",
					editable: false,
					pre: { html: "<i class='bzd-icon-calendar'></i>" },
				});
			},
			nbDaysCurMonth() {
				return new Date(this.date.getFullYear(), this.date.getMonth() + 1, 0).getDate();
			},
			nbDaysPrevMonth() {
				return new Date(this.date.getFullYear(), this.date.getMonth(), 0).getDate();
			},
			yearsMatrix() {
				let matrix = [];
				let index = this.date.getFullYear() - 4;
				for (let rows = 0; rows < 3; ++rows) {
					matrix[rows] = [];
					for (let columns = 0; columns < 3; ++columns, ++index) {
						const date = new Date(new Date(this.date.getTime()).setFullYear(index));
						matrix[rows].push({ current: true, selected: this.date.getFullYear() == index, text: index, date: date });
					}
				}
				return matrix;
			},
			monthsMatrix() {
				let matrix = [];
				let index = 0;
				for (let rows = 0; rows < 6; ++rows) {
					matrix[rows] = [];
					for (let columns = 0; columns < 2; ++columns, ++index) {
						const date = new Date(new Date(this.date.getTime()).setMonth(index));
						matrix[rows].push({
							current: true,
							selected: this.date.getMonth() == index,
							text: this.months[index],
							date: date,
						});
					}
				}
				return matrix;
			},
			daysMatrix() {
				const firstDay = new Date(this.date.getFullYear(), this.date.getMonth(), 1).getDay();

				let matrix = [];
				let rowIndex = -1;
				let day = 0;
				let index = firstDay;
				while (day < this.nbDaysCurMonth) {
					matrix[++rowIndex] = [];
					for (; index < 7 && day < this.nbDaysCurMonth; ++index) {
						++day;
						matrix[rowIndex].push({
							current: true,
							selected: day == this.date.getDate(),
							text: day,
							date: new Date(this.date.getFullYear(), this.date.getMonth(), day),
						});
					}
					index = 0;
				}

				for (let i = 0; i < firstDay; ++i) {
					matrix[0].unshift({ current: false, selected: false, text: this.nbDaysPrevMonth - i });
				}
				for (let i = 1; matrix[rowIndex].length < 7; ++i) {
					matrix[rowIndex].push({ current: false, selected: false, text: i });
				}

				return matrix;
			},
		},
		methods: {
			isValidDate(date) {
				return date instanceof Date && !isNaN(date);
			},
			selectDate(e, date, nextSelect = null) {
				if (nextSelect) {
					e.preventDefault();
					this.select = nextSelect;
				}
				if (date === null) {
					return;
				}
				this.set(date.getTime(), { action: "update" });
			},
			handleKey(/*keyCode*/) {},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/date.scss" as *;
</style>
