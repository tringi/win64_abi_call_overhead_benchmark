# X64 Windows ABI parameter passing overhead
*Measures the horrible overhead of the X64 ABI on Windows*

## The problem

Using modern C++ means leaning towards replacing the legacy of C with new C++ utilities.
Pointer & size arguments with `std::string_view` or `std::span`, plain pointers with `std::optional` or `std::unique_ptr`, etc.
On Windows, due to [ABI](https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170),
these utilities cannot be passed in registers, but must be copied through the stack instead.
This incurs performance penalty.

## This repository

There are numerous anectodatal stories on how large this penalty is.
They vary from fraction of the percent to n-teen percents, which is significant regression.
The fact is, it will vary based on the codebase.
The codebases that don't or can't afford to inline and optimize at the highest level,
and contain high number of small functions, will be affected the most.

This repository contains pathological case of a lof of tiny not inlined functions,
one using `std::span` the other `char*` and `size_t` parameters, being called in partially unrolled loop.
[The bechmark](https://github.com/tringi/win64_abi_call_overhead_benchmark/blob/master/win64_abi_call_overhead_benchmark.cpp)
measures how many of these functions can be called in a second.

## Results

On my Ryzen 5900X:  
The `std::span`-passing function makes **23 billion** calls per seconds.  
The pointer & length passing code makes **91 billion** calls per seconds.  
Almost 4 times as many.

## A proposed solution

Obviously the calling convention for system APIs can't be changed,
but for the functions withing a project it absolutely can.

See the following proposal for further reasoning and references:  
https://github.com/tringi/papers/blob/main/cxx-x64-v2-calling-convention.md
