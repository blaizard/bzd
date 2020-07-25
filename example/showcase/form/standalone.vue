<template>
	<div>
		<div class="bzd-showcase-form-element">
			<Input v-model="input"></Input>
			<pre>{{ JSON.stringify(input) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Password v-model="password"></Password>
			<pre>{{ JSON.stringify(password) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Checkbox v-model="checkbox">My Text</Checkbox>
			<pre>{{ JSON.stringify(checkbox) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Textarea v-model="textarea"></Textarea>
			<pre>{{ JSON.stringify(textarea) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Dropdown v-model="dropdown" :description="{ list: ['yes', 'no', 'maybe'] }"></Dropdown>
			<pre>{{ JSON.stringify(dropdown) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Autocomplete v-model="autocomplete" :description="{ list: (text) => text }"></Autocomplete>
			<pre>{{ JSON.stringify(autocomplete) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<DateElement v-model="date"></DateElement>
			<pre>{{ JSON.stringify(date) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Array v-model="array"></Array>
			<pre>{{ JSON.stringify(array) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Table v-model="table"></Table>
			<pre>{{ JSON.stringify(table) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<File v-model="file" :description="{ upload: '/dummy' }"></File>
			<pre>{{ JSON.stringify(file) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Carousel v-model="carousel" :description="{ list: ['Hello', 'World', 'Great'] }"></Carousel>
			<pre>{{ JSON.stringify(carousel) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<Editor v-model="editor"></Editor>
			<pre>{{ JSON.stringify(editor) }}</pre>
		</div>
		<div class="bzd-showcase-form-element">
			<div>
				<Button content="No action"></Button>
				<Button action="approve" content="Approve"></Button>
				<Button action="cancel" content="Cancel"></Button>
				<Button action="danger" content="Danger"></Button>
			</div>
		</div>
	</div>
</template>

<script>
import Input from "bzd/vue/components/form/element/input.vue";
import Password from "bzd/vue/components/form/element/password.vue";
import Checkbox from "bzd/vue/components/form/element/checkbox.vue";
import Textarea from "bzd/vue/components/form/element/textarea.vue";
import Dropdown from "bzd/vue/components/form/element/dropdown.vue";
import Autocomplete from "bzd/vue/components/form/element/autocomplete.vue";
import Array from "bzd/vue/components/form/element/array.vue";
import Table from "bzd/vue/components/form/element/table.vue";
import File from "bzd/vue/components/form/element/file.vue";
import Carousel from "bzd/vue/components/form/element/carousel.vue";
import Editor from "bzd/vue/components/form/element/editor.vue";
import Button from "bzd/vue/components/form/element/button.vue";
import DateElement from "bzd/vue/components/form/element/date.vue";

export default {
	components: {
		Input,
		Password,
		Checkbox,
		Textarea,
		Dropdown,
		Autocomplete,
		Array,
		Table,
		File,
		Carousel,
		Editor,
		Button,
		DateElement,
	},
	data: function () {
		return {
			input: "hello",
			password: "world",
			textarea: "hello\nworld!",
			dropdown: "yes",
			autocomplete: "",
			date: Date.now(),
			file: [],
			editor: "Hello, I am <b>bold!</b>",
			carousel: 0,
			array: [],
			table: [],
			checkbox: false,
		};
	},
};
</script>
