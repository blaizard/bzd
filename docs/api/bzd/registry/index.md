# [`bzd`](../../index.md)::[`Registry`](../index.md)

## `template<class Interface, class T> class Registry`
Fixed-size registry object.

A registry is a singleton object acting as a key-value store for object instances. The size of the registry must be defined before populating the object, to do so, it is preferable to do it at startup during dynamic variable initialization.

```c++
// Declares a registry which can contain up to 3 doubles
bzd::declare::Registry<double, 3> registry_;
```

Note, this initialization scheme can be imediatly followed by object registrations, as order of global variables in a single translation unit (source file) are initialized in the order in which they are defined.
#### Template
||||
|---:|:---|:---|
|class Interface|None||
|class T|None||

|Function||
|:---|:---|
|[`Registry(const KeyType & str, Args &&... args)`](./index.md)||
|[`get(const KeyType & key)`](./index.md)|Registry accessor.|
------
### `template<class... Args> constexpr Registry(const KeyType & str, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|const KeyType &|str||
|Args &&...|args||
------
### `static constexpr Interface & get(const KeyType & key)`
Registry accessor.
#### Parameters
||||
|---:|:---|:---|
|const KeyType &|key||
