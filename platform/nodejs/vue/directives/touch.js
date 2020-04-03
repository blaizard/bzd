'use strict';

const DRAG_ELEMENT = "drag-element";
const DRAG_PLACEHOLDER = "drag-placeholder";
const DRAG_RECEIVER_ACTIVE = "drag-receiver-active";
const DRAG_OVER_ACTIVE = "drag-over-active";
const DRAG_ACTIVE = "drag-active";
const DRAG_RECEIVER_CONFIG = "data-drag-config";

let originalElt = null;
let originalCoord = null;
let originalTime = 0;
let currentCoord = null;
let originalEltVisibility = null;
let dragElt = null;
let placeholderElt = null;
let coordOffset = null;
let receiverDocument = null;
let receiverCoordOffset = null;
let receiverEltList = null;
let activeReceiverElt = null;
let activeChildElt = -1;
let curConfig = null;
let interval = null;
let preventScrolling = false;

const STATE_IDLE = 0;
const STATE_TRIGGER_DRAG_AFTER_MOVE = 1;
const STATE_INIT = 2;
const STATE_DRAG = 3;
let state = STATE_IDLE;

const eventListenerOptions = {capture: true, passive: false};
const eventIgnoreOptions = {capture: true, passive: false, once: true};

// WebKit requires cancelable `touchmove` events to be added as early as possible
if (typeof window === "object") {
	window.addEventListener("touchmove", (e) => {
		if (preventScrolling) {
			e.preventDefault();
		}
	}, {passive: false});
}

function synchronizeCssStyles(src, destination, recursive) {
	destination.style.cssText = document.defaultView.getComputedStyle(src, "").cssText;

	if (recursive) {
		const vSrcElements = src.childNodes;
		const vDstElements = destination.childNodes;
		for (let i = vSrcElements.length; i--;) {
			const vSrcElement = vSrcElements[i];
			let vDstElement = vDstElements[i];
			if (vSrcElement instanceof HTMLElement) {
				vDstElement.style.cssText = document.defaultView.getComputedStyle(vSrcElement, "").cssText;
			}
		}
	}
}

function isVisible(elt) {
	if (elt.offsetParent === null) return false;
	while (elt && (elt.body === undefined) && (elt.style.opacity === "" || parseFloat(elt.style.opacity) > 0.1)) {
		elt = elt.parentNode;
	}
	// Note: use "elt.body !== undefined" to support iframes
	return (elt.body !== undefined);
}

function isPrimaryButtonOrTouch(e) {
	// Ignore mousedowns on any button other than the left (or primary)
	// mouse button, or when a modifier key is pressed.
	return (e.which === 1 && !e.ctrlKey && !e.altKey)
			|| (e.touches && e.touches.length);
}

function getCoordinates(e) {
	return (e.touches && e.touches.length) 
			? {x: e.touches[0].pageX, y: e.touches[0].pageY}
			: {x: e.pageX, y: e.pageY};
}

function getCurrentDistance() {
	return Math.sqrt((originalCoord.x - currentCoord.x) * (originalCoord.x - currentCoord.x)
			+ (originalCoord.y - currentCoord.y) * (originalCoord.y - currentCoord.y));
}

function scrollOffsetY() {
	return window.pageYOffset || document.documentElement.scrollTop || document.body.scrollTop || 0;
}

function scrollOffsetX() {
	return window.pageXOffset || document.documentElement.scrollLeft || document.body.scrollLeft || 0;
}

function getEltCoordinates(elt) {
	const relativePos = elt.getBoundingClientRect();
	return {
		x: relativePos.left + scrollOffsetX(),
		y: relativePos.top + scrollOffsetY()
	}
}

function removeAllClass(className) {
	const elts = (receiverDocument || document).getElementsByClassName(className);
	while(elts.length > 0){
		elts[0].classList.remove(className);
	}
}

function activateTouch(e, el, config) {

	// Do nothing if the event if not correct
	if (!config.enable || !isPrimaryButtonOrTouch(e)) {
		return;
	}

	// Save the current context
	curConfig = config;
	originalElt = el;
	currentCoord = originalCoord = getCoordinates(e);
	originalTime = new Date().getTime();

	// Setup handler
	document.addEventListener("touchend", dragStop, eventListenerOptions);
	document.addEventListener("touchmove", dragMove, eventListenerOptions);
	document.addEventListener("mouseup", dragStop, eventListenerOptions);
	document.addEventListener("mousemove", dragMove, eventListenerOptions);

	// Set the new state
	state = STATE_TRIGGER_DRAG_AFTER_MOVE;

	// Stop propagation to avoid annoying selections (only if allowClickThrough is disabled)
	if (!curConfig.allowClickThrough) {
		e.preventDefault();
		e.stopPropagation();
	}
}

async function dragStart(e) {
	state = STATE_INIT;

	if (curConfig.disablePointerEvents) {
		Object.assign(document.body.style, {
			"pointer-events": "none"
		});
	}

	if (curConfig.drag) {

		// Set the drag active class on the body
		if (curConfig.bodyDragClass) {
			document.body.classList.add(curConfig.bodyDragClass);
		}

		// Reset the coordinates offset
		receiverCoordOffset = {x: 0, y: 0};
		receiverDocument = document;

		// Set class to all
		if (curConfig.selectorReceiver) {
			if (curConfig.selectorIFrame) {
				let iframe = document.querySelector(curConfig.selectorIFrame);
				// Disable any pointer event to all drag over
				iframe.style['pointer-events'] = "none";
				// Calculate the receiver coordinates offset from the iframe
				const coordIFrameViewPort = iframe.getBoundingClientRect();
				receiverCoordOffset = {
					x: coordIFrameViewPort.left,
					y: coordIFrameViewPort.top
				};
				receiverDocument = iframe.contentWindow.document;
			}
			receiverEltList = [...receiverDocument.querySelectorAll(curConfig.selectorReceiver)];
			// As the selection is made depth-first, we want to have the nested element first in the selection in order
			// to make sure they will be selected if contained into another selector. Hence a quick trick is to reverse
			// this selection.
			receiverEltList = receiverEltList.reverse();
			receiverEltList.forEach((elt) => {
				elt.classList.add(DRAG_RECEIVER_ACTIVE);
			});
			activeReceiverElt = null;
		}
		else {
			activeReceiverElt = originalElt.parentNode;
			activeReceiverElt.classList.add(DRAG_RECEIVER_ACTIVE);
			activeReceiverElt.classList.add(DRAG_OVER_ACTIVE);
			receiverEltList = null;
		}

		// Generate the placeholder element
		{
			if (typeof curConfig.placeholder === "function") {
				placeholderElt = await curConfig.placeholder(originalElt);
			}
			else if (curConfig.placeholder == "clone") {
				placeholderElt = originalElt.cloneNode(true);
				synchronizeCssStyles(originalElt, placeholderElt, /*recursive*/true);
			}
			else if (curConfig.placeholder == "cloneNode") {
				placeholderElt = originalElt.cloneNode(true);
			}
			else {
				placeholderElt = document.createElement("div");
			}
			// Add the class
			placeholderElt.classList.add(DRAG_PLACEHOLDER);
			(curConfig.placeholderClass) && (placeholderElt.classList.add(curConfig.placeholderClass));
			for (const key in curConfig.placeholderCss) {
				placeholderElt.style[key] = curConfig.placeholderCss[key];
			}
		}

		// Generate the drag element
		{
			if (typeof curConfig.dragElt === "function") {
				dragElt = await curConfig.dragElt();
			}
			else if (curConfig.dragElt == "clone") {
				dragElt = originalElt.cloneNode(true);
				synchronizeCssStyles(originalElt, dragElt, /*recursive*/true);
			}
			else if (curConfig.dragElt == "cloneNode") {
				dragElt = originalElt.cloneNode(true);
			}
			else {
				dragElt = document.createElement("div");
			}
			// Add some style
			Object.assign(dragElt.style, {
				position: "fixed",
				'pointer-events': "none",
				'z-index': 99999999,
				'margin-left': 0,
				'margin-top': 0
			}, curConfig.dragEltCss);

			// Add the class
			dragElt.classList.add(DRAG_ELEMENT);
			(curConfig.dragEltClass) && (dragElt.classList.add(curConfig.dragEltClass));

			// Happened it to the dom
			document.body.appendChild(dragElt);
		}

		// Calculate the coordinates offset
		{
			if (curConfig.dragElt == "clone") {
				const coord = getCoordinates(e);
				const pos = getEltCoordinates(originalElt);

				coordOffset = {
					x: coord.x - pos.x,
					y: coord.y - pos.y
				}
			}
			else {
				coordOffset = {
					x: dragElt.offsetWidth / 2,
					y: dragElt.offsetHeight / 2
				}
			}
		}

		if (curConfig.move) {
			originalEltVisibility = originalElt.style.display;
			originalElt.style.display = "none";
		}

		// Reset active variables
		activeChildElt = -1;
	}

	// Stop event propagation while drag is enabled
	document.addEventListener("click", stopEventPropagation, eventIgnoreOptions);
	document.addEventListener("touch", stopEventPropagation, eventIgnoreOptions);

	// Prevent scrolling for touch devices
	preventScrolling = true;

	// Update the state to drag
	state = STATE_DRAG;

	// Call the start drag callback
	if (curConfig.onstartdrag) {
		curConfig.onstartdrag();
	}

	// Trigger the move event to ensure that the drag element is
	// updated at the right position
	dragMove(e);

	if (curConfig.drag) {
		// Run the interval function to detect drop location
		const detectDropZoneCallback = function() {
			//const startTime = performance.now();
			detectDropZone();
			//console.log("detectDropZone took " + Math.round(performance.now() - startTime) + "ms");
			interval = setTimeout(detectDropZoneCallback, 100);
		};
		detectDropZoneCallback();
	}
}

function stopEventPropagation(e) {
	e.stopPropagation();
	e.preventDefault();
}

/**
 * Detect if a point is inside a rectangle given a specific tolerance
 */
function isPointInRectangle(point, rectangle, tolerance) {
	return (rectangle.left - tolerance <= point.x
			&& rectangle.top - tolerance <= point.y
			&& rectangle.left + rectangle.width + tolerance >= point.x
			&& rectangle.top + rectangle.height + tolerance >= point.y);
}

function detectDropZone() {
	const coordDragEltViewPort = dragElt.getBoundingClientRect();
	const coordViewPort = {
		x: coordDragEltViewPort.left + coordOffset.x - receiverCoordOffset.x,
		y: coordDragEltViewPort.top + coordOffset.y - receiverCoordOffset.y
	};

	if (receiverEltList) {
		// Check if drag an active element
		let newActiveReveiverElt = null;
		let newActiveReceiverEltWithTolerance = null;

		// Idenfity the active receiver
		for (const i in receiverEltList) {
			const elt = receiverEltList[i];
			if (isVisible(elt)) {
				const coordEltViewPort = elt.getBoundingClientRect();

				// First choice with no tolerance, use it
				if (isPointInRectangle(coordViewPort, coordEltViewPort, /*tolerance*/0)) {
					newActiveReveiverElt = elt;
					break;
				}
				// This will act as the second guess if none of them matched. Note we use the last match as this will be
				// the outer (not nested) one. And this is what we want for the second guess.
				else if (isPointInRectangle(coordViewPort, coordEltViewPort, curConfig.toleranceReceiverMargin)) {
					newActiveReceiverEltWithTolerance = elt;
				}
			}
		}

		// Use the second guess if there is no first choice
		if (!newActiveReveiverElt && newActiveReceiverEltWithTolerance) {
			newActiveReveiverElt = newActiveReceiverEltWithTolerance;
		}

		// Set or unset the active reciever
		if (newActiveReveiverElt && activeReceiverElt !== newActiveReveiverElt) {
			activeReceiverElt = newActiveReveiverElt;
			removeAllClass(DRAG_OVER_ACTIVE);
			activeReceiverElt.classList.add(DRAG_OVER_ACTIVE);
		}
		else if (activeReceiverElt && !newActiveReveiverElt) {
			activeReceiverElt = newActiveReveiverElt;
			removeAllClass(DRAG_OVER_ACTIVE);
		}
	}

	// If there is an active reciever, update it
	if (activeReceiverElt) {
		let newActiveChildElt = null;
		let isPlaceHolderBefore = false;

		// Check if there is a callback
		const activeReceiverConfig = Object.assign({
			/**
			 * List of indexes to be ignored
			 */
			ignore: []
		}, (activeReceiverElt.hasAttribute(DRAG_RECEIVER_CONFIG)) ? JSON.parse(activeReceiverElt.getAttribute(DRAG_RECEIVER_CONFIG)) : undefined);

		// Identify the active child element
		{
			const childList = activeReceiverElt.childNodes;
			for (let i = 0; i<childList.length; ++i) {
				const child = childList[i];

				// Ignore if this is the placeholder
				if (child.classList.contains(DRAG_PLACEHOLDER)) {
					isPlaceHolderBefore = true;
				}

				// Check if this index is in the ignore list, if so ignore it
				{
					const actualIndex = (isPlaceHolderBefore) ? i - 1 : i;
					if (activeReceiverConfig.ignore.indexOf(actualIndex) !== -1) {
						continue;
					}
				}

				const coordChildViewPort = child.getBoundingClientRect();

				// If the mouse pointer is above the child
				const isOverChild = (coordViewPort.y <= coordChildViewPort.top + coordChildViewPort.height
							&& coordViewPort.x <= coordChildViewPort.left + coordChildViewPort.width);

				if (isOverChild) {

					// If the children is actually the placeholder, do nothing
					if (child.classList.contains(DRAG_PLACEHOLDER)) {
						return;
					}

					if (isPlaceHolderBefore) {

						// Redraw the content by removing the placeholder to estimate the new coordinates
						const prevDisplayProp = placeholderElt.style.display;
						placeholderElt.style.display = "none";

						const coordChildViewPortUpdated = child.getBoundingClientRect();
						const isOverChildUpdated = (coordViewPort.y <= coordChildViewPortUpdated.top + coordChildViewPortUpdated.height
								&& coordViewPort.x <= coordChildViewPortUpdated.left + coordChildViewPortUpdated.width);

						placeholderElt.style.display = prevDisplayProp;

						if (isOverChildUpdated) {
							return;
						}
					}

					newActiveChildElt = child;
					break;
				}
			}
		}

		activeChildElt = newActiveChildElt;

		// Detach and insert the placeholder here
		if (placeholderElt.parentNode) {
			placeholderElt.parentNode.removeChild(placeholderElt);
		}

		// Decide where to place it
		if (activeChildElt) {
			if (isPlaceHolderBefore) {
				if (activeChildElt.nextSibling) {
					activeReceiverElt.insertBefore(placeholderElt, activeChildElt.nextSibling);
				}
				else {
					activeReceiverElt.appendChild(placeholderElt);
				}
			}
			else {
				activeReceiverElt.insertBefore(placeholderElt, activeChildElt);
			}
		}
		else {
			activeReceiverElt.appendChild(placeholderElt);
		}
	}
	// Detach and insert the placeholder here
	else if (!curConfig.keepLastDrop && placeholderElt.parentNode) {
		placeholderElt.parentNode.removeChild(placeholderElt);
		activeReceiverElt = null;
		activeChildElt = -1;
	}
}

function dragMove(e) {

	if (!isPrimaryButtonOrTouch(e)) {
		reset();
		return;
	}

	currentCoord = getCoordinates(e);

	// This is done only if within drag state
	if (state == STATE_DRAG) {
		e.preventDefault();
		e.stopPropagation();

		// Call the callback if set
		if (curConfig.ondrag) {
			curConfig.ondrag(currentCoord.x - originalCoord.x, currentCoord.y - originalCoord.y);
		}

		// Move the element if drag
		if (curConfig.drag) {
			// Calculate coordinate of the element according to the viewPort
			const coordViewPort = {
				x: currentCoord.x - scrollOffsetX(),
				y: currentCoord.y - scrollOffsetY()
			};
			dragElt.style.left = coordViewPort.x - coordOffset.x + "px";
			dragElt.style.top = coordViewPort.y - coordOffset.y + "px";
		}
	}
	else if (state == STATE_TRIGGER_DRAG_AFTER_MOVE) {

		if (!curConfig.allowClickThrough) {
			e.preventDefault();
			e.stopPropagation();
		}

		if (getCurrentDistance() > 5) {
			dragStart(e); 
		}
	}
}

/**
 * Reset all data and stop dragging
 */
function reset() {

	// Reset the user selection
	if (curConfig.disablePointerEvents) {
		Object.assign(document.body.style, {
			"pointer-events": "auto"
		});
	}

	// If it was previously in drag mode, handle the drop
	if (state == STATE_DRAG) {

		if (curConfig.onswipe) {

			const distance = getCurrentDistance();
			if (distance > 10) {

				// Calculate the speed
				const speedPixelPerSecond = distance / ((new Date().getTime() - originalTime) / 1000);

				if (Math.abs(currentCoord.x - originalCoord.x) > Math.abs(currentCoord.y - originalCoord.y)) {
					if (currentCoord.x - originalCoord.x > 0) {
						curConfig.onswipe("right", speedPixelPerSecond);
					}
					else {
						curConfig.onswipe("left", speedPixelPerSecond);
					}
				}
				else {
					if (currentCoord.y - originalCoord.y > 0) {
						curConfig.onswipe("down", speedPixelPerSecond);
					}
					else {
						curConfig.onswipe("up", speedPixelPerSecond);
					}
				}
			}
		}

		if (curConfig.onstopdrag) {
			curConfig.onstopdrag();
		}

		if (curConfig.drag) {

			// If a place is found
			if (placeholderElt.parentNode) {

				// Calculate the current element index
				let elementIndex = 0;
				{
					let curElt = placeholderElt;
					while((curElt = curElt.previousSibling) != null) {
						++elementIndex;
					}
				}
				const eventData = {
					index: elementIndex,
					container: placeholderElt.parentNode,
					args: curConfig.args
				};

				// call the ondrop callback if defined
				if (curConfig.ondrop) {
					curConfig.ondrop(eventData);
				}

				// Trigger the event
				if (curConfig.triggerEvent) {
					const event = new CustomEvent(curConfig.triggerEvent, {
						detail: eventData
					});
					placeholderElt.parentNode.dispatchEvent(event);
				}
			}

			if (curConfig.move) {
				if (!curConfig.nop && placeholderElt.parentNode) {
					// Detach the original element
					originalElt.parentNode.removeChild(originalElt);
					// Paste it at the right place
					placeholderElt.parentNode.insertBefore(originalElt, placeholderElt);
				}
				// Make it visible again
				originalElt.style.display = originalEltVisibility;
			}
		}

		// Reset the drag class
		if (curConfig.bodyDragClass) {
			document.body.classList.remove(curConfig.bodyDragClass);
		}

		// Reset the iframe state if there was any
		if (curConfig.selectorIFrame) {
			let iframe = document.querySelector(curConfig.selectorIFrame);
			iframe.style['pointer-events'] = "auto";
		}
	}

	// Clear the interval
	((interval) && clearInterval(interval)); interval = null;

	// Remove handler
	document.removeEventListener("touchend", dragStop, eventListenerOptions);
	document.removeEventListener("touchmove", dragMove, eventListenerOptions);
	document.removeEventListener("mouseup", dragStop, eventListenerOptions);
	document.removeEventListener("mousemove", dragMove, eventListenerOptions);

	// Those event are fired after, hence we need a short delay
	setTimeout(() => {
		document.removeEventListener("click", stopEventPropagation, eventListenerOptions);
		document.removeEventListener("touch", stopEventPropagation, eventListenerOptions);
	}, 100);

	// Re-enable scrolling
	preventScrolling = false;

	// Reset all classes
	removeAllClass(DRAG_RECEIVER_ACTIVE);
	removeAllClass(DRAG_OVER_ACTIVE);

	// Delete the drag element
	((dragElt) && (dragElt.remove())); dragElt = null;
	((placeholderElt) && (placeholderElt.remove())); placeholderElt = null;

	// Reset the state of the state machine
	state = STATE_IDLE;
}

function dragStop(e) {

	// To allow the option allowClickThrough to go through
	if (!curConfig.allowClickThrough && state != STATE_TRIGGER_DRAG_AFTER_MOVE) {
		e.preventDefault();
		e.stopPropagation();
	}

	// Reset the dragging data
	reset();
}

// Constructor of the Touch object
function Touch(el, config) {
	this.setConfig(config);
	this.el = el;
}

Touch.prototype.setConfig = function (config) {
	this.config = Object.assign({
		/**
		 * Kill switch, must be set to true in order to enable the following events
		 */
		enable: true,
		/**
		 * Boolean to set the drag as enabled or not
		 */
		drag: false,
		/**
		 * The iframe selector. If set, it will asssume that the selector
		 * is within the iframe.
		 */
		selectorIFrame: null,
		/**
		 * The receiver selector. If the selector is null,
		 * the parent element will be used.
		 */
		selectorReceiver: null,
		/**
		 * Class set once the draggin is active. This class is set on the body
		 * of the document. If null, no class will be set.
		 */
		bodyDragClass: "drag-active",
		/**
		 * Can be either null (will generate an empty div),
		 * set to "clone", will clone the original element and its CSS,
		 * set to "cloneNode", will clone the original element only,
		 * set to a callback that will create the element.
		 */
		placeholder: "clone",
		/**
		 * The class to add to the placeholder element
		 */
		placeholderClass: null,
		/**
		 * The css to add to the placeholder element
		 */
		placeholderCss: {},
		/**
		 * Can be either null (will generate an empty div),
		 * set to "clone", will clone the original element and its CSS,
		 * set to "cloneNode", will clone the original element only,
		 * set to a callback that will create the element.
		 */
		dragElt: "clone",
		/**
		 * The class to add to dragging element
		 */
		dragEltClass: null,
		/**
		 * The css to add to dragging element
		 */
		dragEltCss: {},
		/**
		 * Set to true, if the original element should be moved or cloned
		 */
		move: true,
		/**
		 * Do not perform any operation
		 */
		nop: false,
		/**
		 * Let the click events go through
		 */
		allowClickThrough: false,
		/**
		 * Trigger the event drop, if null, no event is triggered
		 */
		triggerEvent: "drop",
		/**
		 * Keep the last drop location even if it is out of a drop zone.
		 */
		keepLastDrop: !Boolean(config.selectorReceiver),
		/**
		 * Tolerance to be allowed on the receiver to be active.
		 * It will define an invisible margin around the receivers that will
		 * be used to detect whether or not the cursor is within the area.
		 */
		toleranceReceiverMargin: 50,
		/**
		 * Disable all pointer events while dragging (including selections, scrolling...)
		 */
		disablePointerEvents: true,
		/**
		 * Arguments to be passed to the callback
		 */
		args: null,
		/**
		 * Detect swipe event and call the associated callback
		 */
		onswipe: null,
		/**
		 * Callback that will be fired each time the mouse moves while dragging
		 */
		ondrag: null,
		/**
		 * Callback fired when a successfull drop happened.
		 * It gets into arguments an object including the index of the child of the container,
		 * the container and the arguments.
		 */
		ondrop: null,
		/**
		 * Will be called at the begining of the drag operation
		 */
		onstartdrag: null,
		/**
		 * Will be called at the end of the drag operation
		 */
		onstopdrag: null
	}, config);
}

Touch.prototype.handleEvent = function (e) {
	activateTouch(e, this.el, this.config);
}

export default function (el, binding) {

	const config = (binding && binding.value) ? binding.value : binding;

	if (!el.directiveTouchInstance) {
		const touch = new Touch(el, config);

		// Associate the object to the element for update
		el.directiveTouchInstance = touch;

		// Get the list of recievers
		el.addEventListener("mousedown", touch, eventListenerOptions);
		el.addEventListener("touchstart", touch, eventListenerOptions);
	}
	else {
		let touch = el.directiveTouchInstance;
		touch.setConfig(config);
	}
}
