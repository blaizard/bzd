# [`bzd`](../../../index.md)::[`interface`](../../index.md)::[`Task`](../index.md)

## `class Task`


|Function||
|:---|:---|
|[`Task(PtrType context, const FctPtrType fct)`](./index.md)||
|[`bind(interface::Stack & stack)`](./index.md)||
|[`start(void ** pointer)`](./index.md)||
|[`yield(Task & nextTask)`](./index.md)||
------
### ` Task(PtrType context, const FctPtrType fct)`

#### Parameters
||||
|---:|:---|:---|
|PtrType|context||
|const FctPtrType|fct||
------
### `void bind(interface::Stack & stack)`
Bind a task to a stack
#### Parameters
||||
|---:|:---|:---|
|interface::Stack &|stack||
------
### `void start(void ** pointer)`

#### Parameters
||||
|---:|:---|:---|
|void **|pointer||
------
### `void yield(Task & nextTask)`
Switch context and run this new task
#### Parameters
||||
|---:|:---|:---|
|Task &|nextTask||
