# [`bzd`](../../index.md)::[`Scheduler`](../index.md)

## `class Scheduler`

### Function
||||
|---:|:---|:---|
|void|[`addTask(bzd::interface::Task & task)`](.)||
|void|[`start()`](.)||
|void|[`yield()`](.)||
||[`SingletonThreadLocalImpl(SingletonThreadLocalImpl const &)`](.)||
|T &|[`getInstance()`](.)||
|void|[`operator=(SingletonThreadLocalImpl const &)`](.)||
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
