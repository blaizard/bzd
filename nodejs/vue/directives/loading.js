import { mask, maskRemove } from "./mask.js";

export default function (el, binding) {
	if (binding.value) {
		mask(el, "bzd-loading");
	}
	else {
		maskRemove(el);
	}
}
