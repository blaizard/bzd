# Coroutines

## Compiler Point of View

### Handle

The thing referred by `bzd::coroutine::impl::coroutine_handle<>` is an object, that refers to the coroutine’s dynamically allocated state.
Thanks to this object you can, for example, resume the coroutine. The coroutine_handle is a templated type, where Promise type is its template argument.

### Frame

Coroutine function is created by the compiler when you call a new coroutine, for example `auto resumable_object = bzd::delay(10_ms);`

```c++
Async<> bzd::delay(args...)
{
    auto* frame = new CoroDelayFrame{args...};
    auto return_object = frame->_promise.get_return_object();

    co_await frame->_promise.initial_suspend();
    try
    {
        // Coroutine body.
    }
    catch (...)
    {
        frame->_promise.unhandled_exception();
    }

final_suspend:
    co_await frame->_promise.final_suspend();
    delete frame;

suspend:
    return return_object;
}
```

Where the coroutine frame looks something like this:

```c++
struct CoroDelayFrame
{
    Async<>::promise_type _promise;
    // storage for argument passed to coroutine.
    // storage for local variables.
    // storage for representation of the current suspension point.
};
```

Where a `co_await` statement looks something like this:

```c++
{
    auto&& awaiter = //<get-awaitable>;
    if (!awaiter.await_ready())
    {
        // <suspend-coroutine>
        if (awaiter.await_suspend(std::coroutine_handle<> p))
        {
            // <return-to-caller-or-resumer>
        }
        // <resume-point>
    }
    awaiter.await_resume();
}
```

Where `<get-awaitable>` is soething like this:

```c++
template<typename P, typename T>
decltype(auto) get_awaitable(P& promise, T&& expr)
{
    if constexpr (has_any_await_transform_member_v<P>)
        return promise.await_transform(static_cast<T&&>(expr));
    else
        return static_cast<T&&>(expr);
}

template<typename Awaitable>
decltype(auto) get_awaiter(Awaitable&& awaitable)
{
    if constexpr (has_member_operator_co_await_v<Awaitable>)
        return static_cast<Awaitable&&>(awaitable).operator co_await();
    else if constexpr (has_non_member_operator_co_await_v<Awaitable&&>)
        return operator co_await(static_cast<Awaitable&&>(awaitable));
    else
        return static_cast<Awaitable&&>(awaitable);
}

// <get-awaitable> -> co_await value;
auto&& awaitable = get_awaitable(promise, static_cast<decltype(value)>(value));
auto&& awaiter = get_awaiter(static_cast<decltype(awaitable)>(awaitable));
```

## Async implementation
