#pragma once

#include "cc/bzd/test/test.hh"

#include <thread>

/// Run a coroutine-based test in an asynchronous context using a multithreaded executor.
#define TEST_ASYNC_MULTITHREAD(testCaseName, testName, nbThreads) BZDTEST_ASYNC_MULTITHREAD_(testCaseName, testName, nbThreads)
