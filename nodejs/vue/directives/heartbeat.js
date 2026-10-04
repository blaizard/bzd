/// The opacity of the dot when it is stale (waiting for the next update).
const staleOpacity = 0.15;
/// The duration of the fade-in when an update is received.
const fadeInMs = 500;

function createDot(el) {
	if (!el.style.position) {
		el.style.position = "relative";
	}
	let dot = el.querySelector(":scope > .bzd-live-dot");
	if (dot === null) {
		dot = document.createElement("div");
		dot.className = "bzd-live-dot";
		Object.assign(dot.style, {
			position: "absolute",
			top: "6px",
			right: "6px",
			width: "8px",
			height: "8px",
			borderRadius: "50%",
			backgroundColor: "#4caf50",
			opacity: String(staleOpacity),
			pointerEvents: "none",
			zIndex: "10",
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

/// Fade the dot to the stale opacity over the given period.
function fadeOut(dot, period) {
	dot.style.transition = "opacity " + period + "ms linear";
	dot.style.opacity = String(staleOpacity);
}

/// Fade the dot in, then fade it out again over the given period.
function unfade(dot, period) {
	clearTimeout(dot.heartbeatTimer);
	dot.style.transition = "opacity " + fadeInMs + "ms ease";
	dot.style.opacity = "1";
	dot.heartbeatTimer = setTimeout(() => {
		fadeOut(dot, period);
	}, fadeInMs);
}

/// Show a small dot in the corner of an element, fading over the expected
/// period. Each time the counter changes (e.g. a server fetch), the dot
/// fades in then fades out again over the period.
///
/// Usage: v-heartbeat="{ counter: <n>, period: <ms> }"
export default {
	mounted(el, binding) {
		if (binding.value?.counter === undefined || binding.value?.period === undefined) {
			return;
		}
		createDot(el);
	},
	updated(el, binding) {
		if (binding.value?.counter === undefined || binding.value?.period === undefined) {
			return;
		}
		if (binding.oldValue !== undefined && binding.value.counter !== binding.oldValue.counter) {
			unfade(createDot(el), binding.value.period);
		}
	},
	beforeUnmount(el) {
		removeDot(el);
	},
};
