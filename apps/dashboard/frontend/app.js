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

/*
See https://stackoverflow.com/questions/50020026/what-is-the-best-vue-router-practice-for-very-large-webapplications

const route: RouteConfig = {
    path: '/some-path',
    component: AppComponent,
    children: [
        getWelcomeRoute(),
        getDashboardRoute()
    ]
};

function getWelcomeRoute(): RouteConfig {
    return {
        name: ROUTE_WELCOME,
        path: '',
        component: WelcomeComponent
    };
}
*/
Vue.use(VueRouter);

new Vue({
	el: '#app',
	render: (h) => h(App),
	router: router
});
