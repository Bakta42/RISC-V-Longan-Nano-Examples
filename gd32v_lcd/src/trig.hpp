/*
    Trigonometric functions for MCUs with limited functionality, power, memory and space in general
*/

/*
* Based on this article: https://www.homeschoolmath.net/teaching/sine_calculator.php
* 
* This is basically Taylor polynomial expansion of function sin(x), where:
* 
* sin(x) = x - x^3/3! + x^5/5! - x^7/7! ...
* One of the purposes of this excercise (in the end most current CPUs can calculate sin quite well or there are libraries to do so and for most of the rest cases sin/cos tables are just fine)
* is to test:
* - how many terms of the polynomial serie is required to get enough precision result
* - how fast it is
* - how it can be optimized (sure it can quite a bit)
* - how to program it
* - what can we learn from it.
* 
* Optimizations:
* clearly, terms are very similar. We have value of x (input) and all next terms use it in powers increasing by two.
* This would simply imply to use x and x^2 as helpful intermediate values. First term is just x, second is x*xsq, third is second_one*xsq.
* So it seems, we need to keep xsq (x squared) and last used power of x (1, 3, 5, ...)
* Same it goes with factorials. we need 1! (1), 3! and basically last_one times n+1 times n+2 resulting in new n!
* Each step then would be:
* (x1, x5, x9th are plus, x3, x7 are minus). Could be achieved by sign = lastsign * -1, with last sign initialized to -1. Or just sign and then create sign for next step
* xpower = lastxpower * xsquared. We can also keep just xpower and start with x, then prepare next one
* nfactorial = lastfactorial * (n + 1) * (n + 2)
* sinx += sign * xpower / nfactorial
* key values: term - index of term (first, ...) x is 1, -x^3/3! is 2(second), ...
* Then n = (term - 1) * 2 + 1 => We actually could use 0 based idx => n = idx * 2 + 1
* xsquared would be x * x
* xpower = x
* nfact = 1
*/

class Trig
{
public:
	template <typename T>
	static T CalcSin(T x);

    template <typename T>
    static T sin(T x);
    template <typename T>
    static T cos(T x);
    template <typename T>
    static T tan(T x);

	static void testSinCalc();
};

