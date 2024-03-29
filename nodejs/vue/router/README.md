# Router

The motivations to create a new router instead of using the official Vue router are the following:

- Low memory footprint
- Integrate with authentication concept.
- Support for nested routers (to be implemented).

## Getting Started

First of all the router is a Vue plugin and must be installed before being able to access its functionality:

```js
import { createApp } from "vue"
import Router from "bzd/vue/router/router.mjs"

const app = createApp(...);

app.use(Router);

app.mount(...);
```

The router must be set in the mounted lifecycle hook of the component, for example:

```js
<RouterComponent name="my-component"></RouterComponent>
...
mounted() {
	this.$route.set({
		component: "my-component",
		routes: [
			{ path: "/dashboard", component: () => import("dashboard.vue") },
			{ path: "/add", component: () => import("add.vue"), authentication: true },
			{ path: "/list/{page:i}", component: () => import("page.vue"), authentication: ["scope1"] }
		],
		fallback: { component: () => import("404.vue") }
	});
},
...
```
