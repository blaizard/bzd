# Core

The core framework is what orchestrates the task and interaction between the data.

Tasks are in fact coroutines. While those tasks can interact with libraries
running in a multithread environement, they all run in a single thread, in order to simplify data synchronization.
However, multiple core instance can run in different thread and communicate in an asynmetric multiprocessing manner.

## Coroutine

The coroutines are based on a stackfull implemenation, for 2 specific reasons: static memory allocation and code clarity.
Those 2 reasons are in fact related: code clarity will come with the tooling supporting coroutines, which today
is lacking. Tooling at term should also be smart enough to support fully static memory allocation for stackless coroutine,
as of today this is not the case, it can be done at the expense of code readbility, which is something I would not like
to compromise on.

Task are in fact coroutines and works as follow:

int compute()
{
	const int i = bzd::await action();
	i++;
	return i * i;
}

main()
{
	compute();
	const int i = bzd::await compute();
}
