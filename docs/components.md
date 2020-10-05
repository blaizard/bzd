# Components

This framework defines a component based development. Components comes at various level of the development and can be asociated to become a larger component.
A component is defined by the following.
- Inputs
- Outputs
- Constraints

Inputs and outputs are handeled by the communication framework which abstracts all implementation details to the user and uses the best communication mean to transfer data from/to components or from/to the outer world.

# System

A system represents a microcontroller and all its peripherals. It contains both drivers that are accessible via the registry and services for background functionalities, like wifi for example.

# Components

Components are defined to acheive a functionality. They take as input values and/or reference to objects and based on this generates one or multiple outputs.

For example, a components that reads data from an ADC channel and send the data periodically via I2C can have the following definition:

- name: MyComponent
- inputs:
	- adc: "type(object)"
	- i2c: "type(object)"
	- frequency: "type(int)"
- outputs:
	- <none>

And will be used as follow:
- components:
	- MyComponent:
		- inputs:
			- adc: "adc_temperature"
			- i2c: "i2c_device"
			- frequency: 10
