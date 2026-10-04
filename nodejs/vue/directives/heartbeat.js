/// The opacity of the triangle when it is stale (waiting for the next update).
const staleOpacity = 0.15;
/// The duration of the fade-in when an update is received.
const fadeInMs = 500;
/// The default color of the triangle.
const defaultColor = "#4caf50";

function createDot(el, color = defaultColor) {
	if (!el.style.position) {
		el.style.position = "relative";
	}
	let dot = el.querySelector(":scope > .bzd-live-dot");
	if (dot === null) {
		dot = document.createElement("div");
		dot.className = "bzd-live-dot";
		Object.assign(dot.style, {
			position: "absolute",
			top: "0",
			right: "0",
			width: "14px",
			height: "14px",
			backgroundColor: color,
			// A corner triangle: the two sides follow the top and right borders
			// of the host element, the third side is the diagonal.
			clipPath: "polygon(0 0, 100% 0, 100% 100%)",
			opacity: String(staleOpacity),
			pointerEvents: "none",
			zIndex: "2",
		});
		el.appendChild(dot);
	}
	return dot;
}

function removeDot(el) {
	const dot = el.querySelector(":scope > .bzd-live-dot");
	if (dot !== null) {
		clearTimeout(dot.heartbeatTimer);
		dot.remove();
	}
}

/// Fade the triangle to the stale opacity over the given period.
function fadeOut(dot, period) {
	dot.style.transition = "opacity " + period + "ms linear";
	dot.style.opacity = String(staleOpacity);
}

/// Fade the triangle in, then fade it out again over the given period.
function unfade(dot, period, color) {
	clearTimeout(dot.heartbeatTimer);
	dot.style.backgroundColor = color;
	dot.style.transition = "opacity " + fadeInMs + "ms ease";
	dot.style.opacity = "1";
	dot.heartbeatTimer = setTimeout(() => {
		fadeOut(dot, period);
	}, fadeInMs);
}

/// Show a small triangle in the top-right corner of an element, fading over
/// the expected period. Each time the counter changes (e.g. a server fetch),
/// the triangle fades in then fades out again over the period.
///
/// Usage: v-heartbeat="{ counter: <n>, period: <ms>, color: <css color> }"
export default {
	mounted(el, binding) {
		if (binding.value?.counter === undefined || binding.value?.period === undefined) {
			return;
		}
		createDot(el, binding.value.color ?? defaultColor);
	},
	updated(el, binding) {
		if (binding.value?.counter === undefined || binding.value?.period === undefined) {
			return;
		}
		if (binding.oldValue !== undefined && binding.value.counter !== binding.oldValue.counter) {
			const color = binding.value.color ?? defaultColor;
			unfade(createDot(el, color), binding.value.period, color);
		}
	},
	beforeUnmount(el) {
		removeDot(el);
	},
};
