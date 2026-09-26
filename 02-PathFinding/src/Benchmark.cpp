#include "Benchmark.h"
#include "Windows.h"
#include <iostream>

std::map<std::string, BenchmarkTest> Benchmark::benchmarkLists;
int64_t Benchmark::frequency;
int64_t Benchmark::endClock;
double Benchmark::average;

Benchmark::Benchmark()
{
}

Benchmark::~Benchmark()
{
}

void Benchmark::Init()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	frequency = li.QuadPart;
}

void Benchmark::StartClock(std::string _name)
{
	if (benchmarkLists.find(_name) == benchmarkLists.end())
	{
		benchmarkLists[_name].name = _name;
		benchmarkLists[_name].high = DBL_MIN;
		benchmarkLists[_name].low = DBL_MAX;
	}
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	benchmarkLists[_name].current = li.QuadPart;
}

void Benchmark::StopClock(std::string _name)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	endClock = li.QuadPart;
	benchmarkLists[_name].timeList.push_back((float)(endClock - benchmarkLists[_name].current) / frequency);
	std::cout << "||Benchmark for: " + _name + " || Results: " + std::to_string(benchmarkLists[_name].timeList[benchmarkLists[_name].timeList.size() - 1]) + " seconds." << std::endl;
	average = 0;
	for (double time : benchmarkLists[_name].timeList)
	{
		average += time;
		if (time < benchmarkLists[_name].low) benchmarkLists[_name].low = time;
		if (time > benchmarkLists[_name].high) benchmarkLists[_name].high = time;
	}
	benchmarkLists[_name].average = average / benchmarkLists[_name].timeList.size();
}

void Benchmark::PrintResults()
{
	std::cout << std::endl;
	std::cout << "=-=-=-=-=-= || BENCHMARK RESULTS || =-=-=-=-=-=" << std::endl;
	std::cout << std::endl;
	for (auto &b : benchmarkLists)
	{
		std::cout << "=-=-=-=-=-= || " + b.second.name + " || =-=-=-=-=-=" << std::endl;
		std::cout << "- AVERAGE : " + std::to_string(b.second.average) << std::endl;
		std::cout << "- HIGHEST : " + std::to_string(b.second.high) << std::endl;
		std::cout << "- LOWEST :  " + std::to_string(b.second.low) << std::endl;
		std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
		std::cout << "                ." << std::endl;
	}
	std::cout << std::endl;
}
