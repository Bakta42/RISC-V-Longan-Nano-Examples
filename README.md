# Longan_GD32VF_examples

## Cloned version from the original sample repository by RST

example project for Longan Nano (GD32VF)
Containing code for Mandelbrot and Cellular automata, adopted from the original examples repo.

Written by Marijn Stollenga

[RST] some minor enhancements, like some color management and trig functions attempt


### [RST] Note on trig functions:
Trig functions work different way on various platforms. Some have CPU instruction, some do not and even if they have, it might be questionable if to use it.
There are many articles on topic, which, especially on embeded devices like MCUs are and moreover those lower end ones without any FPU or SIMD instructions available,
is quite interresting topic leading to some optimized SW implementation.

By looking around I came accross several methods:
1. stdc++ math library
This is a good example, but unfortunately it is not available for GD32VF framework.
2. Taylor polynom approximation
Quite simple to understand, I already implemented one here and it yields resonably good results.
3. Chebyshev approximation
More accurate than Taylor with same number of terms. So, potentinally faster.
4. CORDIC
COrdinate Rotation Digital Computer. I do not know much about this method.

Here are some links:
https://stackoverflow.com/questions/345085/how-do-trigonometric-functions-work/345117#345117
https://mathworld.wolfram.com/ChebyshevApproximationFormula.html
https://en.wikipedia.org/wiki/CORDIC
