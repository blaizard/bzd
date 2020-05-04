"use strict";

let watcherInstance = null;
const watcherPollingPeriodMs = 500;
const defaultPosition = "e";
const positionSequences = {
	w: ["w", "e", "n", "s"],
	e: ["e", "w", "n", "s"],
	n: ["n", "s", "w", "e"],
	s: ["s", "n", "w", "e"]
};

/**
 * Create a unique tooltip element or get the existing one.
 */
function getOrCreateTooltip() {
	// Look for the tooltip
	const eltList = document.getElementsByClassName("irtooltip");
	if (!eltList.length) {
		let container = document.createElement("div");
		container.classList.add("irtooltip");
		Object.assign(container.style, {
			position: "fixed",
			"pointer-events": "none"
		});

		// The text container
		let text = document.createElement("div");
		text.classList.add("irtooltip-text");
		container.appendChild(text);

		window.addEventListener("scroll", tooltipHide, false);
		document.getElementsByTagName("body")[0].appendChild(container);
		return container;
	}

	return eltList[0];
}

/**
 * Hide the curren tooltip
 */
export function tooltipHide() {
	let tooltipElt = getOrCreateTooltip();
	tooltipElt.style.display = "none";

	// Ensure there is no watcher
	clearWatcher();
}

/**
 * Detect if an element is out of screen
 */
function eltOutOfScreen(elt) {
	const rect = elt.getBoundingClientRect();
	return ((rect.x < 0)
			|| (rect.y < 0)
			|| (rect.x + rect.width > window.innerWidth || rect.y + rect.height > window.innerHeight));
}

/**
 * This function will watch an element and if it disepear,
 * it will hide the tooltip.
 */
function setWatcher(elt) {
	// Clear current watcher first
	clearWatcher();

	const hideFct = () => {
		if (!window.getComputedStyle(elt).display) {
			tooltipHide();
		}
		watcherInstance = setTimeout(() => {
			hideFct();
		}, watcherPollingPeriodMs);
	};

	hideFct();
}

/**
 * Reset and clear the current watcher if any.
 */
function clearWatcher() {
	if (watcherInstance) {
		clearInterval(watcherInstance);
		watcherInstance = null;
	}
}

/**
 * Create and show the tooltip
 */
function tooltipFromEvent(e) {
	e.stopPropagation();
	tooltip(e.target);
}

export function tooltipFromCoords(x, y, message, initialPosition = null) {
	const eltMock = {
		getBoundingClientRect() {
			return {
				top: y,
				left: x,
				width: 0,
				height: 0
			};
		},
		getAttribute(/*name*/) {
			return null;
		}
	};
	tooltip(eltMock, message, initialPosition);
}

export function tooltip(elt, message = null, initialPosition = null) {
	message = message || elt.getAttribute("data-irtooltip");
	if (!message) return;

	const initalPosition = initialPosition || elt.getAttribute("data-irtooltip-position") || defaultPosition;

	// Get the tooltip and set the message
	let tooltipElt = getOrCreateTooltip();
	tooltipElt.firstChild.innerHTML = message;

	// Get element coordinates and update the coordinates
	const coordElt = elt.getBoundingClientRect();
	tooltipElt.className = "irtooltip";
	tooltipElt.style.top = coordElt.top + "px";
	tooltipElt.style.left = coordElt.left + "px";
	tooltipElt.style.marginLeft = 0;
	tooltipElt.style.marginTop = 0;
	tooltipElt.style.display = "block";
	tooltipElt.style.whiteSpace = "nowrap";

	// Calculate the coordinates of the tooltip
	const coordTooltip = tooltipElt.getBoundingClientRect();

	// Create the position list
	let positionList = ["e", "s", "w", "n"];
	if (positionSequences[initalPosition]) {
		positionList = positionSequences[initalPosition].slice();
	}
	else if (initalPosition.indexOf(",") !== -1) {
		positionList = initalPosition.split(",");
	}

	do {
		let position = positionList.shift();

		switch (position) {
		case "e":
			tooltipElt.style.marginLeft = coordElt.width + "px";
			tooltipElt.style.marginTop = ((coordElt.height - coordTooltip.height)/2) + "px";
			tooltipElt.className = "irtooltip irtooltip-e";
			break;
		case "s":
			tooltipElt.style.marginLeft = ((coordElt.width - coordTooltip.width)/2) + "px";
			tooltipElt.style.marginTop = coordElt.height + "px";
			tooltipElt.className = "irtooltip irtooltip-s";
			break;
		case "w":
			tooltipElt.style.marginLeft = -coordTooltip.width + "px";
			tooltipElt.style.marginTop = ((coordElt.height - coordTooltip.height)/2) + "px";
			tooltipElt.className = "irtooltip irtooltip-w";
			break;
		case "n":
			tooltipElt.style.marginLeft = ((coordElt.width - coordTooltip.width)/2) + "px";
			tooltipElt.style.marginTop = -coordTooltip.height + "px";
			tooltipElt.className = "irtooltip irtooltip-n";
			break;
		}

	} while (eltOutOfScreen(tooltipElt) && positionList.length);

	// Set the watcher on this element
	setWatcher(elt);
}

export default function (el, binding) {
	const config = Object.assign({
		/**
		 * The text to display
		 */
		text: undefined,
		/**
		 * The position or positions the tooltip can take (by order of priority)
		 */
		position: defaultPosition
	}, binding.value);

	el.addEventListener("mouseenter", tooltipFromEvent, false);
	el.addEventListener("mouseleave", tooltipHide, false);

	// Set the attributes
	if (config.text) {
		el.setAttribute("data-irtooltip", config.text);
	}
	else {
		el.removeAttribute("data-irtooltip");
	}

	if (config.position == defaultPosition) {
		el.removeAttribute("data-irtooltip-position");
	}
	else if (typeof config.position === "string") {
		el.setAttribute("data-irtooltip-position", config.position);
	}
	else {
		el.setAttribute("data-irtooltip-position", config.position.join(","));
	}
}
