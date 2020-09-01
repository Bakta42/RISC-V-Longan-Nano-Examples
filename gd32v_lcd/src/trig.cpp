/*
    Trigonometric functions for MCUs with limited functionality, power, memory and space in general
*/

#include "trig.hpp"

#include <cstdint>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>

template <typename T>
T Trig::CalcSin(T x) {
	T xsquared = x * x;
	T term = x;
	T sinx = x;
	T oldsinx = -1;
	for (int idx = 1; (idx < 12) && (sinx != oldsinx); idx++) {
		oldsinx = sinx;
		term = (term * (-1) * xsquared) / (2 * (uint64_t)idx * (2 * (uint64_t)idx + 1));
		sinx += term;
	}
	return sinx;
}

float floatCalcSin(float x) {
	return Trig::CalcSin(x);
}

double doubleCalcSin(double x) {
	return Trig::CalcSin(x);
}

template <typename T>
T testSinCalcExec(uint32_t iterations) {
	T sinestep = M_PI_2 / iterations;
	T x = 0;
	for (uint32_t i = 0; i < iterations; ++i)
	{
		x += Trig::CalcSin(sinestep * i);
	}
	return x;
}

template <typename T>
T testSinCPU(uint32_t iterations) {
	T sinestep = M_PI_2 / iterations;
	T x = 0;
	for (uint32_t i = 0; i < iterations; ++i)
	{
		x += sin(sinestep * i);
	}
	return x;
}

void Trig::testSinCalc()
{
	const uint32_t iterations = 1000000;
	double testX = 0;
	// CalcSine float
	auto begin = std::chrono::high_resolution_clock::now();
	testX = testSinCalcExec<float>(iterations);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	std::cout << "Float sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;

	// float sin built-in
	begin = std::chrono::high_resolution_clock::now();
	testX = testSinCPU<float>(iterations);
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	std::cout << "Float(lib) sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;

	// double sin calculated
	begin = std::chrono::high_resolution_clock::now();
	testX = testSinCalcExec<double>(iterations);
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	std::cout << "Double sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;

	// double sin built-in
	begin = std::chrono::high_resolution_clock::now();
	testX = testSinCPU<double>(iterations);
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	std::cout << "Double(lib) sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;
}
