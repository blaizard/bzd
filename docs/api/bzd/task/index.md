# [`bzd`](../../index.md)::[`Task`](../index.md)

## `template<class T, class... Args> class Task`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||

|Function||
|:---|:---|
|[`Task(const T & callable)`](./index.md)||
|[`wrapper()`](./index.md)||
|[`bind(interface::Stack & stack)`](./index.md)||
|[`start(void ** pointer)`](./index.md)||
|[`yield(Task & nextTask)`](./index.md)||
------
### ` Task(const T & callable)`

#### Parameters
||||
|---:|:---|:---|
|const T &|callable||
------
### `template<class R> static void wrapper()`

#### Template
||||
|---:|:---|:---|
|class R|None||
------
### `void bind(interface::Stack & stack)`
*From bzd::interface::Task*

Bind a task to a stack
#### Parameters
||||
|---:|:---|:---|
|interface::Stack &|stack||
------
### `void start(void ** pointer)`
*From bzd::interface::Task*


#### Parameters
||||
|---:|:---|:---|
|void **|pointer||
------
### `void yield(Task & nextTask)`
*From bzd::interface::Task*

Switch context and run this new task
#### Parameters
||||
|---:|:---|:---|
|Task &|nextTask||
