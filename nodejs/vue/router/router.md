# Router

A request router is available to ease.
It support nested router that recieve remaining path during the dispatch processed.
This approach helps isolating components in a Vue application.

The motivations to create a new router instead of using the official Vue router are the following:

- Support for nested routers
- Low memory footprint

## Getting Started

First of all the router is a Vue plugin and must be installed before being able to access its fuctionality:

```js
import { createApp } from "vue"
import Router from "bzd/vue/router/router.mjs"

const app = createApp(...);

app.use(Router);

app.mount(...);
```

The router must be set in the mounted lifecycle hook of the component, for example:

```js
...
mounted() {
	this.$routerSet({
		ref: "my-component",
		routes: [
			{ path: "/dashboard", nested: true, component: () => import("dashboard.vue") },
			{ path: "/add", component: () => import("add.vue") },
			{ path: "/list/{page:i}", component: () => import("page.vue") }
		],
		fallback: { component: () => import("404.vue") }
	});
},
...
```

Only one router can be used per component, if you attempt to setup a second one, an error will be raised.
