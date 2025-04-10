<template>
	<div v-if="nbObjects > 0">
		<template v-if="nbObjects <= max">
			<div class="text" v-for="(value, inputName) in input">
				<div class="name">
					<span class="small">{{ name.substring(0, 3) }}.</span>{{ inputName.toUpperCase() }}
				</div>
				<div class="value">
					{{ value }}
				</div>
			</div>
		</template>
		<template v-else>
			<div class="text">
				<div class="name">{{ name }}</div>
				<div class="value">
					{{ aggregatedValue }}
				</div>
			</div>
		</template>
	</div>
</template>

<script>
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

	const Exception = ExceptionFactory("plugin", "system_monitor", "text");

	export default {
		props: {
			input: { type: Object, mandatory: true },
			name: { type: String, mandatory: true },
			max: { type: Number, default: 2 },
		},
		computed: {
			nbObjects() {
				return Object.keys(this.input).length;
			},
			aggregatedValue() {
				return Object.values(this.input).join(", ");
			},
		},
	};
</script>

<style lang="scss">
	.text {
		width: 100%;
		display: flex;
		flex-direction: row;
		flex-wrap: nowrap;
		font-size: 0.8em;
		margin-bottom: 5px;

		.name {
			width: 30%;
			text-align: right;
			padding-right: 10px;
			overflow: hidden;
			height: 1.5em;

			.small {
				font-size: 0.6em;
			}
		}
		.value {
			flex: 1;
			overflow: hidden;
			white-space: nowrap;
			position: relative;
			isolation: isolate;
			height: 1.5em;
			line-height: 1.5em;
			text-overflow: ellipsis;
			padding-left: 4px;

			&:before {
				content: " ";
				position: absolute;
				left: 0;
				top: 0;
				bottom: 0;
				right: 0;
				background-color: currentColor;
				opacity: 0.1;
			}

			&:after {
				content: " ";
			}
		}
	}
</style>
