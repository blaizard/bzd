"use strict";

// Used to generated unique Ids
let elementUid = 0;

// Map to keep track of current observers
let observers = {};

function autoResize(el) {

	let uid = getElementId(el);

	// Register the observer if not already done
	if (!observers[uid]) {
		uid = registerObserver(el);
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
		let rect = el.getBoundingClientRect();
		const rectParent = el.parentElement.getBoundingClientRect();

		// If the same hash has previously been processed, ignore
		if (observers[uid].hash === generateHash(rect, rectParent)) {
			return;
		}

		let maxFontSize = rectParent.height;
		let minFontSize = 3;
		let fontSize = parseInt(el.style.fontSize) || Math.floor((maxFontSize + minFontSize) / 2);

		el.style.transition = "";
		el.style.display = "inline-block";

		// Compute the new size by binary search
		do {
			el.style.fontSize = fontSize + "px";
			rect = el.getBoundingClientRect();

			// Update the font size boundaries
			if (rect.height > rectParent.height || rect.width > rectParent.width) {
				maxFontSize = fontSize;
			}
			else {
				minFontSize = fontSize;
			}

			fontSize = Math.floor((maxFontSize + minFontSize) / 2);

			// console.log(uid + "; max: [" + rectParent.width + "px, " + rectParent.height + "px], size: [" + rect.width + "px, " + rect.height + "px], font: [" + minFontSize + "px, " + maxFontSize + "px]");
			// The difference is not an integer, therefore we must check the upper limit (hence the >=2 and not > 1)
		} while (Math.abs(maxFontSize - minFontSize) >= 2);

		el.style.fontSize = minFontSize + "px";
		el.style.opacity = 1;

		// No need to check if there is an observer there must be at this point.
		// Set the new hash
		observers[uid].hash = generateHash(rect, rectParent);
	}, 10);
}

function registerObserver(el) {
	const uid = setElementId(el);

	// Hide the element the fist time
	el.style.opacity = 0;

	if (typeof ResizeObserver !== "undefined") {
		observers[uid] = {
			obj: new ResizeObserver((entries) => {
				autoResize(el);
			}),
			hash: -1,
			instance: null
		}
		observers[uid].obj.observe(el);
		observers[uid].obj.observe(el.parentElement);
	}
	else {
		observers[uid] = {
			obj: setInterval(() => {
				autoResize(el);
			}, 1000),
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
	return el.getAttribute("data-auto-size") || null;
}

function setElementId(el) {
	const uid = "autosize-" + (++elementUid);
	el.setAttribute("data-auto-size", uid);
	return uid;
}

function generateHash(rect, rectParent) {
	return [rect.width, rect.height, rectParent.width, rectParent.height].reduce((n, arg) => (n * 1000 + arg), 0);
}

export default {
	inserted: function(el) {
		autoResize(el);
	},
	componentUpdated: function(el) {
		autoResize(el);
	},
	unbind: function (el) {
		const uid = getElementId(el);
		unregisterObserver(uid);
	}
}
