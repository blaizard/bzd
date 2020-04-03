'use strict';

export default function (el, binding) {
	// Delete the first child if it is aready a mask
	if (el.firstChild.classList && el.firstChild.classList.contains("component-directive-mask")) {
		el.firstChild.remove();
	}

	// Set position of the element
	el.style.position = "relative";

	// If set to true, set the mask
	if (binding.value) {
		// Create the mask
		let mask = document.createElement("div");
		mask.className = "component-directive-mask";
		mask.style.cssText = "position: absolute; top: 0, left: 0; width: 100%; height: 100%;";

		// Edge does not support prepend, hence use this workaround
		if (el.firstChild) {
			el.insertBefore(mask, el.firstChild);
		}
		else {
			el.appendChild(mask);
		}
	}
}
