# [`bzd`](../../../index.md)::[`interface`](../../index.md)::[`Task`](../index.md)

## `class Task`

### Function
||||
|---:|:---|:---|
||[`Task(CtxPtrType context, const FctPtrType fct)`](./index.md)||
|void|[`bind(interface::Stack & stack)`](./index.md)||
|void|[`start(void ** pointer)`](./index.md)||
|void|[`yield(Task & nextTask)`](./index.md)||
------
### ` Task(CtxPtrType context, const FctPtrType fct)`

#### Parameters
||||
|---:|:---|:---|
|CtxPtrType|context||
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
