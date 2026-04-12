#include "math.h"

int my_multiply(int a, int b) {
    int result = 0;
    int neg = 0;
    if (a < 0) { a = -a; neg = !neg; }
    if (b < 0) { b = -b; neg = !neg; }
    while (b--) result += a;
    return neg ? -result : result;
}

int my_divide(int a, int b) {
    if (b == 0) return 0; /* guard against div by zero */
    int neg = 0;
    if (a < 0) { a = -a; neg = !neg; }
    if (b < 0) { b = -b; neg = !neg; }
    int q = 0;
    while (a >= b) { a -= b; q++; }
    return neg ? -q : q;
}

int my_modulo(int a, int b) {
    if (b == 0) return 0;
    int neg = (a < 0);
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (a >= b) a -= b;
    return neg ? -a : a;
}

int my_abs(int a) {
    return (a < 0) ? -a : a;
}

int my_clamp(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}
