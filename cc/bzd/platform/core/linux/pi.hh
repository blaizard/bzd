#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

// object for sampling random numbers
class Rnd
{
public:
	Rnd() : gen{std::random_device()()}, dist{0, 1.0} {}
	double operator()() { return dist(gen); }

protected:
	std::mt19937 gen;							 // random number generator
	std::uniform_real_distribution<double> dist; // uniform distribution
};

// function to launch in parallel.
bzd::Async<bzd::UInt64Type> worker(bzd::UInt64Type total)
{
	Rnd rnd{};
	bzd::UInt64Type n = 0;

	for (bzd::UInt64Type s = 0; s < total; s++)
	{
		double x = rnd();
		double y = rnd();
		if (x * x + y * y <= 1)
		{
			++n;
		}
	}

	co_return n;
}

template <bzd::SizeType nbCores>
bzd::Async<double> calculatePi()
{
	const bzd::UInt64Type goal = 100000000;

	bzd::Array<bzd::SizeType, nbCores> goals;

	for (bzd::SizeType i = 0; i < nbCores; i++)
	{
		bzd::UInt64Type goal_per_thread = goal / nbCores;
		// Correct on last one to get the correct total.
		if (i == nbCores - 1)
		{
			goal_per_thread = goal - (goal_per_thread * (nbCores - 1));
		}

		goals[i] = goal_per_thread;
	}

	auto promise0 = worker(goals[0]);
	auto promise1 = worker(goals[1]);

	const auto result = co_await bzd::async::all(promise0, promise1);

	bzd::UInt64Type sum = 0;
	bzd::constexprForContainerInc(result, [&sum](auto& item) { sum += item.value(); });

	const double pi = 4 * sum / static_cast<double>(goal);
	std::cout << "PI: " << pi << std::endl;

	co_return pi;
}
