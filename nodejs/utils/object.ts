/// Return the difference between object1 and object2.
export function objectDifference(object1: Record<string, unknown>, object2: Record<string, unknown>): Record<string, unknown> {
    const keys = new Set([...Object.keys(object1), ...Object.keys(object2)]);
    let difference: Record<string, unknown> = {};
    for (const key of keys) {
        if (key in object1 && !(key in object2)) {
            difference[key] = object1[key];
        } else if (!(key in object1) && key in object2) {
            difference[key] = object2[key];
        }
        // The key is in both but they hhave different types
        else if (typeof object1[key] !== typeof object2[key]) {
            difference[key] = object2[key];
        }
        // The key is in both and points to objects
        else if (
            typeof object1[key] === "object" &&
            !Array.isArray(object1[key]) &&
            !Array.isArray(object2[key])
        ) {
            difference[key] = objectDifference(object1[key] as Record<string, unknown>, object2[key] as Record<string, unknown>);
        }
        // The key is in both and points to different values
        else if (object1[key] !== object2[key]) {
            difference[key] = object2[key];
        }
    }
    return difference;
}

/// Simple object check.
///
/// \param item The variable to be checked.
///
/// \return true if this is an object, false otherwise.
export function isObject(item: unknown): item is Record<string, unknown> {
    return item !== null && typeof item === "object" && !Array.isArray(item);
}

/// Deep merge two or more objects.
///
/// \param target
/// \param ...sources
///
/// \return The merged object.
export function deepMerge<T extends Record<string, any>>(target: T, ...sources: Record<string, unknown>[]): T {
    if (!sources.length) return target;
    const source = sources.shift()!;

    if (isObject(target) && isObject(source)) {
        for (const key in source) {
            if (isObject(source[key])) {
                if (!target[key]) Object.assign(target, { [key]: {} });
                deepMerge(target[key], source[key]);
            } else {
                Object.assign(target, { [key]: source[key] });
            }
        }
    }

    return deepMerge(target, ...sources);
}

/// Extends an existing object with more methods/variables.
export function extendObject<T extends object, U extends object>(object: T, extension: U): T & U {
    return new Proxy(object, {
        get: function (target, prop, receiver) {
            // If the property exists on the original object, return it.
            if (prop in target) {
                return Reflect.get(target, prop, receiver);
            }

            // If the property exists in the extensions object, return it (bound to extensions).
            if (prop in extension) {
                const extensionValue = (extension as Record<PropertyKey, any>)[prop];
                if (typeof extensionValue === "function") {
                    // Bind the extension function to the proxy (receiver).
                    return extensionValue.bind(receiver);
                } else {
                    return extensionValue;
                }
            }

            // If the property doesn't exist in either, return undefined.
            return undefined;
        },
    }) as T & U;
}