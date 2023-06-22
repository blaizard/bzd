# Plugins

Plugins can spawn a frontend and/or a backend entity.

## Backend

```js
export default {
    // Cached entries.
	cache: {
        // The name of the cache.
		"my.name": {
            /// The function that fetches the cache once expired.
            ///
            /// \param previous The previous value of the cache.
			fetch: async (previous) => { return ...; },
            // The validity time of the cache in ms.
            timeout: 12 * 1000,
        }
    },
    // The data returned for this specific plugin to the frontend.
	fetch: async (data, cache) => { return ...; },
    // List of tasks to run in background.
	tasks: {
		"default": {
			task: async (cache) => { ... },
			period: 60 * 1000,
		}
	},
    // List of events or function to expose from this plugin.
    // To be implemented.
	events: {
		async play(data, cache) { ... },
    }
}
```
