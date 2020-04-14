# [`bzd`](../../index.md)::[`Registry`](../index.md)

## `template<class Interface> class Registry`
Fixed-size registry object.

A registry is a singleton object acting as a key-value store for object instances. The size of the registry must be defined before populating the object, to do so, it is preferable to do it at startup during dynamic variable initialization.

```c++
// Declares a registry which can contain up to 3 doubles
bzd::Registry<double>::Declare<3> registry_;
```

The following snipet shows how to register objects within this registry. Note that no more of N objects (number specified during registry declaration) can be added.

```c++
// Creates and insert 3 objects within the registry
bzd::Registry<double>::Register<> registry_{"A", 42};
bzd::Registry<double>::Register<> registry_{"B", -1};
bzd::Registry<double>::Register<> registry_{"C", 3.14};
```

Note, this initialization scheme can be imediatly followed by object registrations, as order of global variables in a single translation unit (source file) are initialized in the order in which they are defined.
#### Template
||||
|---:|:---|:---|
|class Interface|None||

|Class||
|:---|:---|
|[`Declare`](declare/index.md)|Declaration object for a fixed-size registry object.|
|[`Register`](register/index.md)|Register a new object to its registery previously delcared.|

|Function||
|:---|:---|
|[`get(const KeyType & key)`](./index.md)|Registry accessor.|
|[`getOrCreate(const KeyType & key)`](./index.md)|Get a ressource or create one if it does not exists. In order to be created, the ressource must have a default constructor. This is a limitation to ensure reproductability.|
------
### `template<SizeType Capacity> class Declare`
Declaration object for a fixed-size registry object.
#### Template
||||
|---:|:---|:---|
|SizeType|Capacity||
------
### `template<class T> class Register`
Register a new object to its registery previously delcared.
#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `static constexpr Interface & get(const KeyType & key)`
Registry accessor.
#### Parameters
||||
|---:|:---|:---|
|const KeyType &|key||
------
### `template<class T> static Interface & getOrCreate(const KeyType & key)`
Get a ressource or create one if it does not exists. In order to be created, the ressource must have a default constructor. This is a limitation to ensure reproductability.
#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const KeyType &|key||
