<template>
    <a @click="handleClick">
        <slot></slot>
    </a>
</template>

<script>
	"use strict"

	export default {
        props: {
            link: {type: String, mandatory: true}
        },
        methods: {
            handleClick() {
                this.$bzdRouterLink(this.link);
            }
        }
	}
</script>
