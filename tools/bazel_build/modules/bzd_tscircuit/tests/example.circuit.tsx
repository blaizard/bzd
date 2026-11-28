import { BlinkerLed } from "./components/blinker_led";

export default () => (
	<board width="20mm" height="20mm">
		<resistor name="R1" resistance="1k" footprint="0402" />
		<BlinkerLed />
	</board>
);
