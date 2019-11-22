# [`bzd`](../../index.md)::[`Scheduler`](../index.md)

## `class Scheduler`

### Function
||||
|---:|:---|:---|
|void|[`addTask(bzd::interface::Task & task)`](./index.md)||
|void|[`start()`](./index.md)||
|void|[`yield()`](./index.md)||
||[`SingletonThreadLocalImpl(SingletonThreadLocalImpl const &)`](./index.md)||
|T &|[`getInstance()`](./index.md)||
|void|[`operator=(SingletonThreadLocalImpl const &)`](./index.md)||
------
### `void addTask(bzd::interface::Task & task)`
Add a new task to the scheduler
#### Parameters
||||
|---:|:---|:---|
|bzd::interface::Task &|task||
------
### `void start()`

------
### `void yield()`

------
### ` SingletonThreadLocalImpl(SingletonThreadLocalImpl const &)`
*From bzd::SingletonThreadLocalImpl*


#### Parameters
||||
|---:|:---|:---|
|SingletonThreadLocalImpl const &|None||
------
### `static T & getInstance()`
*From bzd::SingletonThreadLocalImpl*


------
### `void operator=(SingletonThreadLocalImpl const &)`
*From bzd::SingletonThreadLocalImpl*


#### Parameters
||||
|---:|:---|:---|
|SingletonThreadLocalImpl const &|None||
