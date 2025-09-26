#include <Windows.h>
#include <cstdlib>
#include <span>

std::size_t r1 = 0;
std::size_t r2 = 0;

template <char X> __declspec(noinline)
std::span <char> f1 (std::span <char> a, std::span <char> b) {
    ++r1;
    ++a [0];
    b [0] += X;
    if (a [0] & X)
        return a;
    else
        return b;
}

template <char X> __declspec(noinline)
char * f2 (char * a, std::size_t aN, char * b, std::size_t bN) {
    ++r2;
    ++a [0];
    b [0] += X;
    if (a [0] & X)
        return a;
    else
        return b;
}

template <unsigned X> __declspec(noinline)
std::span <char> roll1 (std::span <char> a, std::span <char> b) {
    f1 <X> (a, b);
    return roll1 <X - 1> (b, a);
}
template <> __declspec(noinline)
std::span <char> roll1 <0> (std::span <char> a, std::span <char> b) {
    return b;
}

template <unsigned X> __declspec(noinline)
char * roll2 (char * a, std::size_t aN, char * b, std::size_t bN) {
    f2 <X> (a, aN, b, bN);
    return roll2 <X - 1> (b, bN, a, aN);
}
template <> __declspec(noinline)
char * roll2 <0> (char * a, std::size_t aN, char * b, std::size_t bN) {
    return b;
}

int main () {
    constexpr std::size_t Unrolls = 255u;
    char text [256];
    for (char c : text) c = std::rand () % 256;

    auto t1 = GetTickCount64 ();
    while (r1 < 100'000'000) {
        roll1 <Unrolls> (text, text);
    }
    printf ("std::span: %.2f G/s\n", Unrolls * r1 / double (GetTickCount64 () - t1) / 1000000.0);

    for (char c : text) c = std::rand () % 256;

    auto t2 = GetTickCount64 ();
    while (r2 < 100'000'000) {
        roll2 <Unrolls> (text, 256, text, 256);
    }
    printf ("separated: %.2f G/s\n", Unrolls * r2 / double (GetTickCount64 () - t2) / 1000000.0);

    return 0;
}
