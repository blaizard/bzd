"use strict";

function maskEvent(e) {
	e.stopPropagation();
}

export function maskRemove(el) {
	// Delete the first child if it is aready a mask
	if (el.firstChild.classList && el.firstChild.classList.contains("bzd-mask")) {
		el.firstChild.remove();
	}
}

export function mask(el, className = "") {
	// Remove any existing mask
	maskRemove(el);

	// Set position of not already explicitly set
	if (!el.style.position) {
		el.style.position = "relative";
	}

	// Create the mask
	let mask = document.createElement("div");
	mask.className = "bzd-mask " + className;
	mask.style.cssText = "position: absolute; top: 0, left: 0; width: 100%; height: 100%;";
	mask.onclick = maskEvent;

	// Edge does not support prepend, hence use this workaround
	if (el.firstChild) {
		el.insertBefore(mask, el.firstChild);
	}
	else {
		el.appendChild(mask);
	}
}

export default function (el, binding) {
	// If set to true, set the mask
	if (binding.value) {
		mask(el);
	}
	else {
		maskRemove(el);
	}
}
