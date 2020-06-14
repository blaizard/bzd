<template>
    <a @click="handleClick">
        <slot></slot>
    </a>
</template>

<script>


export default {
	props: {
		link: {type: String, mandatory: true}
	},
	methods: {
		handleClick() {
			this.$routerDispatch(this.link);
		}
	}
};
</script>
