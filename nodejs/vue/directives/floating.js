'use strict';

function update(el, config) {

	// Update the config
	config = Object.assign({
		/**
		 * Can also accept "cover"
		 */
		position: "nne",
		inside: false,
		matchSize: false,
		/**
		 * Setting this to false will base the position on the
		 * element size when needed. If set to true, it will
		 * rely on the parent element size.
		 */
		relyOnParentSize: true,
		/**
		 * If set, change the zIndex
		 */
		zIndex: 0
	}, config);

	// Set the z-index if needed
	el.style.zIndex = (config.zIndex) ? config.zIndex : "";

	// Set the same dimensions
	{
		// Reset width and height
		el.style.width = "auto";
		el.style.height = "auto";
		// Update the width and height if necessary
		if (config.matchSize) {
			switch (config.position) {
			case "nnw":
			case "n":
			case "nne":
			case "ssw":
			case "s":
			case "sse":
				el.style.width = "100%";
				break;
			case "nww":
			case "w":
			case "sww":
			case "nee":
			case "e":
			case "see":
				el.style.height = "100%";
				break;
			}
		}
	}

	// Update the position
	{
		let left = 0;
		let top = 0;
		const parentWidth = el.parentNode.offsetWidth;
		const parentHeight = el.parentNode.offsetHeight;
		const eltWidth = el.offsetWidth;
		const eltHeight = el.offsetHeight;

		// Reset data
		el.style.left = "auto";
		el.style.top = "auto";
		el.style.right = "auto";
		el.style.bottom = "auto";
		el.style.marginTop = 0;
		el.style.marginLeft = 0;
		el.style.marginRight = 0;
		el.style.marginBottom = 0;

		// Set the position
		switch (config.position) {
		case "nnw":
		case "nww":
			break;
		case "n":
			el.style.left = (parentWidth - eltWidth) / 2 + "px";
			break;
		case "nne":
		case "nee":
			el.style.right = 0;
			break;
		case "e":
			el.style.right = 0;
			el.style.top = (parentHeight - eltHeight) / 2 + "px";
			break;
		case "sse":
		case "see":
			el.style.right = 0;
			el.style.bottom = 0;
			break;
		case "s":
			el.style.left = (parentWidth - eltWidth) / 2 + "px";
			el.style.bottom = 0;
			break;
		case "ssw":
		case "sww":
			el.style.left = 0;
			el.style.bottom = 0;
			break;
		case "w":
			el.style.top = (parentHeight - eltHeight) / 2 + "px";
			break;
		}

		// Rely on the parent sizes instead
		if (config.relyOnParentSize) {
			switch (config.position) {
			case "nne":
			case "nee":
			case "e":
				el.style.left = parentWidth + "px";
				break;
			case "sse":
			case "see":
				el.style.left = parentWidth + "px";
				el.style.top = parentHeight + "px";
				break;
			case "s":
			case "ssw":
			case "sww":
				el.style.top = parentHeight + "px";
				break;
			}
		}

		// Handle inside / outside flag
		switch (config.position) {
		case "nnw":
		case "n":
		case "nne":
			if (!config.inside) {
				el.style.marginTop = -eltHeight + "px";
			}
			break;
		case "ssw":
		case "s":
		case "sse":
			if (!config.relyOnParentSize && !config.inside) {
				el.style.marginBottom = -eltHeight + "px";
			}
			else if (config.relyOnParentSize && config.inside) {
				el.style.marginTop = -eltHeight + "px";
			}
			break;
		case "nww":
		case "w":
		case "sww":
			if (!config.inside) {
				el.style.marginLeft = -eltWidth + "px";
			}
			break;
		case "nee":
		case "e":
		case "see":
			if (!config.relyOnParentSize && !config.inside) {
				el.style.marginRight = -eltWidth + "px";
			}
			else if (config.relyOnParentSize && config.inside) {
				el.style.marginLeft = -eltWidth + "px";
			}
			break;
		}
	}

	if (config.position == "cover") {
		el.style.top = 0;
		el.style.left = 0;
		el.style.width = "100%";
		el.style.height = "100%";
	}
}

export default {
	inserted(el, binding) {
		// Set parent node position
		el.parentNode.style.position = "relative";
		el.style.cssText += "position: absolute; padding: 0; margin: 0; display: inline-block;";
		update(el, binding.value);
	},
	update(el, binding) {
		update(el, binding.value);
	}
}
