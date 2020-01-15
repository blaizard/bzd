# [`bzd`](../../index.md)::[`Registry`](../index.md)

## `template<class Interface> class Registry`
Fixed-size registry object.

A registry is a singleton object acting as a key-value store for object instances. The size of the registry must be defined before populating the object, to do so, it is preferable to do it at startup during dynamic variable initialization.

```c++
// Declares a registry which can contain up to 3 doubles
bzd::Registry<double>::Declare<3> registry_;
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
