export default {
	mounted: function (el, binding) {
		const contextKey = binding.expression;

		// Create the context for this directive
		el.dataHoverChildren = {
			handleSelect: (e) => {
				if (e.target.parentNode === el) {
					binding.instance[contextKey] = Array.from(el.children).indexOf(e.target);
				}
			},
			handleUnselect: (e) => {
				if (e.target.parentNode === el) {
					if (binding.instance[contextKey] === Array.from(el.children).indexOf(e.target)) {
						binding.instance[contextKey] = -1;
					}
				}
			},
		};

		el.addEventListener("click", el.dataHoverChildren.handleSelect, true);
		el.addEventListener("mouseenter", el.dataHoverChildren.handleSelect, true);
		el.addEventListener("mouseleave", el.dataHoverChildren.handleUnselect, true);
	},
	unmounted: function (el) {
		el.removeEventListener("mouseleave", el.dataHoverChildren.handleUnselect, true);
		el.removeEventListener("mouseenter", el.dataHoverChildren.handleSelect, true);
		el.removeEventListener("click", el.dataHoverChildren.handleSelect, true);
	},
};
