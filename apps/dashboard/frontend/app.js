"use strict"

import Vue from "vue"
import App from "[frontend]/app.vue"
import Router from "[bzd]/vue/router/router.js"

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

Vue.use(Router);

new Vue({
	el: '#app',
	render: (h) => h(App),
});
