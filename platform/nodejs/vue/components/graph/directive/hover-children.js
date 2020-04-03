export default {
	bind: function(el, binding, vnode) {
		const contextKey = binding.expression;

		// Create the context for this directive
		el.dataHoverChildren = {
			handleSelect: (e) => {
				if (e.target.parentNode === el) {
					vnode.context[contextKey] = Array.from(el.children).indexOf(e.target);
				}
			},
			handleUnselect: (e) => {
				if (e.target.parentNode === el) {
					if (vnode.context[contextKey] === Array.from(el.children).indexOf(e.target)) {
						vnode.context[contextKey] = -1;
					}
				}
			}
		};

		el.addEventListener("click", el.dataHoverChildren.handleSelect, true);
		el.addEventListener("mouseenter", el.dataHoverChildren.handleSelect, true);
		el.addEventListener("mouseleave", el.dataHoverChildren.handleUnselect, true);
	},
	unbind: function (el) {
		el.removeEventListener("mouseleave", el.dataHoverChildren.handleUnselect, true);
		el.removeEventListener("mouseenter", el.dataHoverChildren.handleSelect, true);
		el.removeEventListener("click", el.dataHoverChildren.handleSelect, true);
	}
}
