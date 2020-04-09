"use strict"

import Vue from "vue"
import VueRouter from "vue-router"
import App from "[frontend]/app.vue"

import AddTile from "[frontend]/add_tile.vue"
import Dashboard from "[frontend]/dashboard.vue"

const router = new VueRouter({
	routes: [
		{ path: '/add', component: AddTile },
		{ path: '/dashboard', component: Dashboard }
	]
});

Vue.use(VueRouter);

new Vue({
	el: '#app',
	render: (h) => h(App),
	router: router
});
