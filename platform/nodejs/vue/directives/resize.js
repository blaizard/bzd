"use strict";

// Used to generated unique Ids
let elementUid = 0;

// Map to keep track of current observers
let observers = {};

function onResizeProcess(el, callback) {

	let uid = getElementId(el);

	// Register the observer if not already done
	if (callback) {
		if (observers[uid])
		{
			unregisterObserver(uid);
		}
		uid = registerObserver(el, callback);
	}

	// If this element is registered, clear the previous timeout if any and set this one.
	// This is done to avoid any duplicated calls and keep only the latest one valid.
	clearTimeout(observers[uid].instance);

	// Make everything asynchronous to avoid any execution time limit from the ResizeObserver
	observers[uid].instance = setTimeout(() => {

		// If the element has been destroyed in the mean time, remove it and ignore this request
		if (!el.parentElement) {
			unregisterObserver(uid);
			return;
		}

		// At this point we can ensure that a valid observer context is present
		const rect = el.getBoundingClientRect();
		const hash = rect.width * 10000 + rect.height;

		// If the same hash has previously been processed, ignore
		if (observers[uid].hash === hash) {
			return;
		}
		observers[uid].hash = hash;

		// Do work here
		observers[uid].callback(rect.width, rect.height);
	}, 10);
}

function registerObserver(el, callback) {
	const uid = setElementId(el);

	if (typeof ResizeObserver !== "undefined") {
		observers[uid] = {
			obj: new ResizeObserver((entries) => {
				onResizeProcess(el);
			}),
			callback: callback,
			hash: -1,
			instance: null
		}
		observers[uid].obj.observe(el);
	}
	else {
		observers[uid] = {
			obj: setInterval(() => {
				onResizeProcess(el);
			}, 1000),
			callback: callback,
			hash: -1,
			instance: null
		}
	}

	return uid;
}

function unregisterObserver(uid) {
	if (observers[uid]) {
		clearTimeout(observers[uid].instance);
		if (typeof ResizeObserver !== "undefined") {
			observers[uid].obj.disconnect();
		}
		else {
			clearInterval(observers[uid].obj);
		}
		delete observers[uid];
	}
}

function getElementId(el) {
	return el.getAttribute("data-irresize") || null;
}

function setElementId(el) {
	const uid = "autosize-" + (++elementUid);
	el.setAttribute("data-irresize", uid);
	return uid;
}

export default {
	inserted: function(el, binding) {
		onResizeProcess(el, binding.value);
	},
	componentUpdated: function(el, binding) {
		onResizeProcess(el, binding.value);
	},
	unbind: function (el) {
		const uid = getElementId(el);
		unregisterObserver(uid);
	}
}
