/*
    Trigonometric functions for MCUs with limited functionality, power, memory and space in general
*/

#include "trig.h"

#include <cstdint>
//#include <chrono>
#define _USE_MATH_DEFINES
//#include <math.h>
#include <cmath>
// #include <iostream>

template <typename T>
T sin6(T x) {
	static T const a0 = +1.0;
	static T const a1 = -1.666666666666580809419428987894207e-1;
	static T const a2 = +8.333333333262716094425037738346873e-3;
	static T const a3 = -1.984126982005911439283646346964929e-4;
	static T const a4 = +2.755731607338689220657382272783309e-6;
	static T const a5 = -2.505185130214293595900283001271652e-8;
	static T const a6 = +1.604729591825977403374012010065495e-10;
	static T const a7 = -7.364589573262279913270651228486670e-13;

    T x2 = x * x;
    T x4 = x2 * x2;
    T x8 = x4 * x4;
    T A = a0 + x2 * (a1 + x2 * (a2 + x2 * a3));
    T B = a4 + x2 * (a5 + x2 * (a6 + x2 * a7));
    return x * (A + x8 * B);
} 

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
		//x += Trig::CalcSin(sinestep * i);
		x += sin6(sinestep * i);
	}
	return x;
}

// template <typename T>
// T testSinCPU(uint32_t iterations) {
// 	T sinestep = M_PI_2 / iterations;
// 	T x = 0;
// 	for (uint32_t i = 0; i < iterations; ++i)
// 	{
// 		x += sin(sinestep * i);
// 	}
// 	return x;
// }

float Trig::cos(float x) {
	float fract = std::abs(std::fmod(x, M_PI)) - M_PI_2;
    bool piCountEven = ((int)(x / M_PI)) % 2 == 0;
	if(piCountEven) fract = -fract;
	return sin6(fract);
}

void Trig::testSinCalc()
{
// 	const uint32_t iterations = 1000000;
// 	double testX = 0;
// 	// CalcSine float
// 	auto begin = std::chrono::high_resolution_clock::now();
// 	testX = testSinCalcExec<float>(iterations);
// 	auto end = std::chrono::high_resolution_clock::now();
// 	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
// //	std::cout << "Float sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;

// 	// // float sin built-in
// 	// begin = std::chrono::high_resolution_clock::now();
// 	// testX = testSinCPU<float>(iterations);
// 	// end = std::chrono::high_resolution_clock::now();
// 	// duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
// 	// std::cout << "Float(lib) sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;

// 	// double sin calculated
// 	begin = std::chrono::high_resolution_clock::now();
// 	testX = testSinCalcExec<double>(iterations);
// 	end = std::chrono::high_resolution_clock::now();
// 	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
// //	std::cout << "Double sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;

// 	// // double sin built-in
// 	// begin = std::chrono::high_resolution_clock::now();
// 	// testX = testSinCPU<double>(iterations);
// 	// end = std::chrono::high_resolution_clock::now();
// 	// duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
// 	// std::cout << "Double(lib) sin(x) test: " << duration << "ns total, average : " << duration / iterations << "ns." << " test Value: " << testX << std::endl;
}

extern "C" {
	float mbcos(float x) {
		return Trig::cos(x);
	}
}
